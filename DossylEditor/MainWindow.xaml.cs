using DossylEditor.GameProject;
using System;
using System.Collections.Generic;
using System.ComponentModel;
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
using System.Windows.Media.Imaging;
using System.Windows.Navigation;

namespace DossylEditor {
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window {
        public MainWindow() {
            InitializeComponent();
            Loaded += OnMainWindowLoaded;
            Closing += OnMainWindowClosing;
        }

		public static string DossylPath { get; private set; } // set via env and user prompt

		private void OnMainWindowClosing(object? sender, CancelEventArgs e) {
            Closing -= OnMainWindowClosing;
            Project.Current?.Unload();
        }
        private void OnMainWindowLoaded(object sender, RoutedEventArgs e) {
            Loaded -= OnMainWindowLoaded;
			GetEnginePath();
            OpenProjectBrowserDialog();
        }

		private void GetEnginePath() {
			var dossylPath = Environment.GetEnvironmentVariable("DOSSYL_ENGINE", EnvironmentVariableTarget.User);
			if (dossylPath == null || !Directory.Exists(Path.Combine(dossylPath, @"Engine\EngineAPI"))) {
				var dlg = new EnginePathDialog();
				if (dlg.ShowDialog() == true) {
					DossylPath = dlg.DossylPath;
					Environment.SetEnvironmentVariable("DOSSYL_ENGINE", DossylPath.ToUpper(), EnvironmentVariableTarget.User);
				}
				else {
					Application.Current.Shutdown();
				}
			}
			else {
				DossylPath = dossylPath;
			}
		}

		private void OpenProjectBrowserDialog() {
            var projectBroswer = new ProjectBrowserDialog();
            if (projectBroswer.ShowDialog() == false || projectBroswer.DataContext == null) { // user hit X or no project was created/opened
                Application.Current.Shutdown();
            }
            else { // open project
                Project.Current?.Unload();
                DataContext = projectBroswer.DataContext;
            }
        }
    }
}
