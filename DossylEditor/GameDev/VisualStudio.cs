using DossylEditor.Utilities;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices.ComTypes;
using System.Runtime.InteropServices;
using System.IO;
using DossylEditor.GameProject;

namespace DossylEditor.GameDev {
    static class VisualStudio {

		public static bool BuildSucceeded { get; private set; } = true;
		public static bool BuildDone { get; private set; } = true;

		private static EnvDTE80.DTE2 _vsInstance = null;
        private static readonly string _progId = "VisualStudio.DTE.17.0"; // grab vs 2022 only (would be nice to open this using whatever windows associates with the .sln extension

		// like visual studio and unreal engine do. maybe improvement for later
		[DllImport("ole32.dll")]
        private static extern int CreateBindCtx(uint reserved, out IBindCtx ppbc);
        [DllImport("ole32.dll")]
        private static extern int GetRunningObjectTable(uint reserved, out IRunningObjectTable pprot);
        public static void OpenVisualStudio(string solutionPath) {
            IRunningObjectTable rot = null;
            IEnumMoniker monikerTable = null;
            IBindCtx bindCtx = null;
            try {
                if (_vsInstance == null) {
                    // find and open visual studio
                    var hResult = GetRunningObjectTable(0, out rot);
                    if (hResult < 0 || rot == null) throw new COMException($"GetRunningObjectTable() returned HRESUILT: {hResult:X8}");

                    rot.EnumRunning(out monikerTable);
                    monikerTable.Reset();

                    hResult = CreateBindCtx(0, out bindCtx);
                    if (hResult < 0 || bindCtx == null) throw new COMException($"CreateBindCtx() returned HRESUILT: {hResult:X8}");

                    IMoniker[] currentMoniker = new IMoniker[1];
                    while (monikerTable.Next(1, currentMoniker, IntPtr.Zero) == 0) {
                        string name = string.Empty;
                        currentMoniker[0]?.GetDisplayName(bindCtx, null, out name);

                        if (name.Contains(_progId)) { // found instance but might not be the one with game solution open
                            hResult = rot.GetObject(currentMoniker[0], out object obj);
                            if (hResult < 0 || obj == null) throw new COMException($"Running object table's GetObject() returned HRESUILT: {hResult:X8}");

                            EnvDTE80.DTE2 dte = obj as EnvDTE80.DTE2;
                            var solutionName = dte.Solution.FullName;
                            if (solutionName == solutionPath) { // check if names are the same as the one supplied to this method
                                _vsInstance = dte; // set for later to avoid having to look again
                                break;
                            }
                        }
                    }
                    

                    if (_vsInstance == null) {
                        Type visualStudioType = Type.GetTypeFromProgID(_progId, true);
                        _vsInstance = Activator.CreateInstance(visualStudioType) as EnvDTE80.DTE2;
                    }
                }
            } catch(Exception ex) {
                Debug.Write(ex.Message);
                Logger.Log(MessageType.Error, "failed to open Visual Studio");
            } finally {
                if (monikerTable != null) Marshal.ReleaseComObject(monikerTable);
                if (rot != null) Marshal.ReleaseComObject(rot);
                if (bindCtx != null) Marshal.ReleaseComObject(bindCtx);
            }
        }

        public static void CloseVisualStudio() { 
            if (_vsInstance?.Solution.IsOpen == true) {
                _vsInstance.ExecuteCommand("File.SaveAll");
                _vsInstance.Solution.Close(true);
            }
            _vsInstance?.Quit();
        }

		public static bool AddFilesToSolution(String solution, String projectName, String[] files) {
			Debug.Assert(files?.Length > 0);
			OpenVisualStudio(solution);
			try { 
				if (_vsInstance != null) {
					if (!_vsInstance.Solution.IsOpen) _vsInstance.Solution.Open(solution);
					else _vsInstance.ExecuteCommand("File.SaveAll");

					foreach (EnvDTE.Project project in _vsInstance.Solution.Projects) { 
						if (project.UniqueName.Contains(projectName)) { 
							foreach (var file in files) {
								project.ProjectItems.AddFromFile(file);
							}
						}
					}
					var cpp = files.FirstOrDefault(x => Path.GetExtension(x) == ".cpp");
					if (!string.IsNullOrEmpty(cpp)) {
						_vsInstance.ItemOperations.OpenFile(cpp, EnvDTE.Constants.vsViewKindTextView).Visible = true;
					}
					_vsInstance.MainWindow.Activate();
					_vsInstance.MainWindow.Visible = true;
				}
			} catch(Exception ex) {
				Debug.WriteLine(ex.Message);
				Logger.Log(MessageType.Error, "failed to add files to Visual Studio Project");
				return false;
			}
			return true;
		}
		private static void OnBuildSolutionBegin(String project, String projectConfig, String platform, String solutionConfig) {
			Logger.Log(MessageType.Info, $"Building {project}, {projectConfig}, {platform}, {solutionConfig}");
		}
		private static void OnBuildSolutionDone(String project, String projectConfig, String platform, String solutionConfig, bool success) {
			if (BuildDone) return;

			if (success) Logger.Log(MessageType.Info, $"Building {projectConfig} configuration succeeded");
			else Logger.Log(MessageType.Error, $"Building {projectConfig} configuration failed");

			BuildDone = true;
			BuildSucceeded = success;
		}
		public static bool IsDebugging() {
			bool result = false;
			for (int i = 0; i < 3; ++i) {
				try {
					result = _vsInstance != null &&
						(_vsInstance.Debugger.CurrentProgram != null || _vsInstance.Debugger.CurrentMode == EnvDTE.dbgDebugMode.dbgRunMode);
					break;
				} catch (Exception ex) {
					Debug.WriteLine(ex.Message);
					if (!result) System.Threading.Thread.Sleep(1000);
				}
			}
			return result;
		}

		internal static void BuildSolution(Project project, String configName, bool showWindow = true) {
			if (VisualStudio.IsDebugging()) {
				Logger.Log(MessageType.Error, "Visual Studio is currently running a process.");
				return;
			}

			OpenVisualStudio(project.Solution);
			BuildDone = false;
			BuildSucceeded = false;
			

			for (int i = 0; i < 3; ++i) { // simple but bad. MessageFilter to deal with busy COM state would be better
				try {
					if (!_vsInstance.Solution.IsOpen)
						_vsInstance.Solution.Open(project.Solution);
					_vsInstance.MainWindow.Visible = showWindow;

					_vsInstance.Events.BuildEvents.OnBuildProjConfigBegin += OnBuildSolutionBegin;
					_vsInstance.Events.BuildEvents.OnBuildProjConfigDone += OnBuildSolutionDone;

					try {
						// each new build requires a new pdb file (previous ones conflict in name). They're now marked with timestamp but don't need old ones, so just remove
						foreach (var pdbFile in Directory.GetFiles(Path.Combine($"{project.Path}", $@"x64\{configName}"), "*.pdb"))
							File.Delete(pdbFile);
					} catch (Exception ex) { // if current dll is still loaded, attempted deletion might throw due to access not found
						Debug.WriteLine(ex.Message); // we just catch in that case and overall should delete old enough pdb files so they dont stack up
					}

					_vsInstance.Solution.SolutionBuild.SolutionConfigurations.Item(configName).Activate();
					_vsInstance.Solution.SolutionBuild.Build(true); // must wait to avoid race condition due to event callbacks
					//_vsInstance.ExecuteCommand("Build.BuildSolution");
					break;
				} catch (Exception ex) {
					Debug.WriteLine(ex.Message);
					Debug.WriteLine($"Attempt {i}: failed to build {project.Name}");
					System.Threading.Thread.Sleep(1000);
				}
			}
		}
	}
}
