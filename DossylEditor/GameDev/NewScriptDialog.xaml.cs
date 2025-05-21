using DossylEditor.GameProject;
using DossylEditor.Utilities;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Imaging;

namespace DossylEditor.GameDev {
    /// <summary>
    /// Interaction logic for NewScriptDialog.xaml
    /// </summary>
    public partial class NewScriptDialog : Window {

		private static readonly string _cppCode = @"
#include ""{0}.hpp""

namespace {1} {{
	REGISTER_SCRIPT({0});
	auto {0}::beginPlay() -> void {{
	
	}}
	auto {0}::update(float dt) -> void {{
	
	}}
}} // namespace {1}";

		private static readonly string _hppCode = @"#pragma once

namespace {1} {{

	class {0} : public Dossyl::Script::EntityScript {{
	public:
		constexpr explicit {0}(Dossyl::GameEntity::Entity entity) :
			Dossyl::Script::EntityScript(entity)
		{{}}
		
		auto beginPlay() -> void override;
		auto update(float) -> void override;

	private:
	}};
}} // namespace {1}";

		public NewScriptDialog() {
            InitializeComponent();
			Owner = Application.Current.MainWindow;
			scriptPath.Text = @"GameCode\";
        }

		private static readonly string _namespace = GetNamespaceFromProjectName();

		private static string GetNamespaceFromProjectName() {
			var temp = Project.Current.Name;
			var projectName = string.Empty;
			bool capsNext = true;
			foreach (char c in temp) {
				if (c == ' ') {
					capsNext = true;
					continue;
				}
				if (capsNext) {
					projectName += Char.ToUpper(c);
					capsNext = false;
				}
				else projectName += c; // could ToLower this, but this allows acronyms (ie, if it's already caps, it stays caps)
			}

			return projectName;
		}

		private bool Validate() {
			bool isValid = false;
			var name = scriptName.Text.Trim();
			var path = scriptPath.Text.Trim();
			string errorMsg = string.Empty;
			if (string.IsNullOrEmpty(name)) {
				errorMsg = "Type in a script name.";
			}
			else if (name.IndexOfAny(Path.GetInvalidFileNameChars()) != -1 || name.Any(x => char.IsWhiteSpace(x))) {
				errorMsg = "Invalid character(s) used in script name.";
			}
			else if (string.IsNullOrEmpty(path)) {
				errorMsg = "Select a valid script folder";
			}
			else if (path.IndexOfAny(Path.GetInvalidPathChars()) != -1) {
				errorMsg = "Invalid character(s) used in script path.";
			}
			else if (!Path.GetFullPath(Path.Combine(Project.Current.Path, path)).Contains(Path.Combine(Project.Current.Path, @"GameCode\"))) {
				errorMsg = "Script must be added to (a sub-folder of) GameCode.";
			}
			else if (File.Exists(Path.GetFullPath(Path.Combine(Path.Combine(Project.Current.Path, path), $"{name}.cpp"))) ||
				File.Exists(Path.GetFullPath(Path.Combine(Path.Combine(Project.Current.Path, path), $"{name}.hpp")))
			) {
				errorMsg = $"script {name} already exists in this folder.";
			}
			else {
				isValid = true;
			}

			if (!isValid) {
				messageTextBlock.Foreground = FindResource("Editor.RedBrush") as Brush;
			}
			else {
				messageTextBlock.Foreground = FindResource("Editor.FontBrush") as Brush;
			}
			messageTextBlock.Text = errorMsg;
			return isValid;
		}

        private void OnScriptName_TextBox_TextChanged(Object sender, TextChangedEventArgs e) {
            if (!Validate()) return;

			var name = scriptName.Text.Trim();
			var project = Project.Current;
			messageTextBlock.Text = $"{name}.hpp and {name}.cpp will be added to {Project.Current.Name}.";
        }

        private void OnScriptPath_TextBox_TextChanged(Object sender, TextChangedEventArgs e) {
			Validate();
        }

        private async void OnCreate_Button_Click(Object sender, RoutedEventArgs e) {
			if (!Validate()) return;
			IsEnabled = false;
			busyAnimation.Opacity = 0;
			busyAnimation.Visibility = Visibility.Visible;
			DoubleAnimation fadeIn = new DoubleAnimation(0, 1, new Duration(TimeSpan.FromMilliseconds(500)));
			busyAnimation.BeginAnimation(OpacityProperty, fadeIn);

			try {
				var name = scriptName.Text.Trim();
				var path = Path.GetFullPath(Path.Combine(Project.Current.Path, scriptPath.Text.Trim()));
				var solution = Project.Current.Solution;
				var projectName = Project.Current.Name;
				await Task.Run(() => CreateScript(name, path, solution, projectName));
			} catch(Exception ex) {
				Debug.WriteLine(ex.Message);
				Logger.Log(MessageType.Error, $"Failed to create script {scriptName.Text}");
			} finally {
				DoubleAnimation fadeOut = new DoubleAnimation(1, 0, new Duration(TimeSpan.FromMilliseconds(200)));
				fadeOut.Completed += (s, e) => {
					busyAnimation.Opacity = 0;
					busyAnimation.Visibility = Visibility.Hidden;
					Close();
				};
				busyAnimation.BeginAnimation(OpacityProperty, fadeOut);
			}
        }

		private void CreateScript(string name, string path, string solution, string projectName) {
			if (!Directory.Exists(path)) Directory.CreateDirectory(path);
			var cpp = Path.GetFullPath(Path.Combine(path, $"{name}.cpp"));
			var hpp = Path.GetFullPath(Path.Combine(path, $"{name}.hpp"));

			using (var sw = File.CreateText(cpp)) {
				sw.Write(string.Format(_cppCode, name, _namespace));
			}
			using (var sw = File.CreateText(hpp)) {
				sw.Write(string.Format(_hppCode, name, _namespace));
			}

			string[] files = new string[] { cpp, hpp };

			for (int i = 0; i < 3; ++i) {
				if (!VisualStudio.AddFilesToSolution(solution, projectName, files))
					System.Threading.Thread.Sleep(1000); // if failed to add files, wait a second and try again
				else break;
			}
		}
	}
}
