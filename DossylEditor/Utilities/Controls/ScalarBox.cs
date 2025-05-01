using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace DossylEditor.Utilities.Controls {
    class ScalarBox : NumberBox {
		static ScalarBox() { // allows <Style TargetType="{x:Type ScalarBox}"> in xaml files
			DefaultStyleKeyProperty.OverrideMetadata(
				typeof(ScalarBox),
				new FrameworkPropertyMetadata(typeof(ScalarBox))
			);
		}
	}
}
