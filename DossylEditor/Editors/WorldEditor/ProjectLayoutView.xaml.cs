﻿using DossylEditor.Components;
using DossylEditor.GameProject;
using DossylEditor.Utilities;
using System;
using System.Collections.Generic;
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

namespace DossylEditor.Editors {
    /// <summary>
    /// Interaction logic for ProjectLayoutView.xaml
    /// </summary>
    public partial class ProjectLayoutView : UserControl {
        public ProjectLayoutView() {
            InitializeComponent();
        }

        private void OnAddGameEntity_Button_Click(object sender, RoutedEventArgs e) {
            var btn = sender as Button;
            var vm = btn.DataContext as Scene;
            vm.AddGameEntityCommand.Execute(new GameEntity(vm) { Name = "Empty Game Entity" });
        }

        private void OnGameEntitities_ListBox_SelectionChanged(object sender, SelectionChangedEventArgs e) {
            var listBox = (sender as ListBox);
            var newSelection = listBox.SelectedItems.Cast<GameEntity>().ToList();
            var previousSelection = newSelection.Except(e.AddedItems.Cast<GameEntity>()).Concat(e.RemovedItems.Cast<GameEntity>()).ToList();
            Project.undoRedo.Add(new UndoRedoAction(
                () => {
                    listBox.UnselectAll();
                    previousSelection.ForEach(
                        x => (listBox.ItemContainerGenerator.ContainerFromItem(x) as ListBoxItem).IsSelected = true
                    );
                },
                () => {
                    listBox.UnselectAll();
                    newSelection.ForEach(
                        x => (listBox.ItemContainerGenerator.ContainerFromItem(x) as ListBoxItem).IsSelected = true
                    );
                },
                "Selection Changed"
            ));
            MSGameEntity msEntity = null;
            if (newSelection.Any()) {
                msEntity = new MSGameEntity(newSelection);
            }
            GameEntityView.Instance.DataContext = msEntity;
        }
    }
}
