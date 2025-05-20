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

namespace DossylEditor.GameDev {
    static class VisualStudio {
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
	}
}
