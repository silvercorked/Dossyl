﻿using DossylEditor.GameDev;
using DossylEditor.GameProject;
using System;
using System.Collections.Generic;
using System.Collections.Specialized;
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

namespace DossylEditor.Editors
{
    /// <summary>
    /// Interaction logic for WorldEditorView.xaml
    /// </summary>
    public partial class WorldEditorView : UserControl
    {
        public WorldEditorView()
        {
            InitializeComponent();
            Loaded += OnWorldEditorViewLoaded;
        }

        private void OnWorldEditorViewLoaded(object sender, RoutedEventArgs e) {
            Loaded -= OnWorldEditorViewLoaded;
            Focus();
        }

		private void OnNewScript_Button_Click(Object sender, RoutedEventArgs e) {
			var dlg = new NewScriptDialog();
			dlg.ShowDialog(); // handles file creation on it's own
        }
    }
}
