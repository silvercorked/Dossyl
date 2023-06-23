using DossylEditor.GameProject;
using System;
using System.Collections.Generic;
using System.ComponentModel;
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
using System.Windows.Shapes;

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
        private void OnMainWindowClosing(object? sender, CancelEventArgs e) {
            Closing -= OnMainWindowClosing;
            Project.Current?.Unload();
        }
        private void OnMainWindowLoaded(object sender, RoutedEventArgs e) {
            Loaded -= OnMainWindowLoaded;
            OpenProjectBrowserDialog();
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
