﻿using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace DossylEditor.Utilities.Controls {
	[TemplatePart(Name = "PART_textblock", Type = typeof(TextBlock))]
	[TemplatePart(Name = "PART_textBox", Type = typeof(TextBox))]
	class NumberBox : Control {

		private double _originalValue;
		private double _mouseXStart;
		private bool _captured = false;
		private bool _valueChangedOnDrag = false;
		private double _multiplier;

		public double Multiplier {
			get => (double)GetValue(MultiplierProperty);
			set => SetValue(MultiplierProperty, value);
		}

		public static readonly DependencyProperty MultiplierProperty =
			DependencyProperty.Register(
				nameof(Multiplier),
				typeof(double),
				typeof(NumberBox),
				new PropertyMetadata(1.0)
			);
		public string Value {
			get => (string) GetValue(ValueProperty);
			set => SetValue(ValueProperty, value);
		}

		public static readonly DependencyProperty ValueProperty =
			DependencyProperty.Register(
				nameof(Value),
				typeof(string),
				typeof(NumberBox),
				new FrameworkPropertyMetadata(null, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault)
			);

		public override void OnApplyTemplate() {
			base.OnApplyTemplate();
			
			if (GetTemplateChild("PART_textBlock") is TextBlock textBlock) {
				textBlock.MouseLeftButtonDown += OnTextBlock_Mouse_LBD;
				textBlock.MouseLeftButtonUp += OnTextBlock_Mouse_LBU;
				textBlock.MouseMove += OnTextBlock_Mouse_Move;
			}
		}

		private void OnTextBlock_Mouse_LBD(Object sender, MouseButtonEventArgs e) {
			double.TryParse(Value, out _originalValue);
			Mouse.Capture(sender as UIElement);
			_captured = true;
			_valueChangedOnDrag = false;
			e.Handled = true;
			Focus();

			_mouseXStart = e.GetPosition(this).X;
		}

		private void OnTextBlock_Mouse_LBU(Object sender, MouseButtonEventArgs e) {
			if (_captured) {
				Mouse.Capture(null);
				_captured = false;
				e.Handled = true;
				if (!_valueChangedOnDrag && GetTemplateChild("PART_textBox") is TextBox textBox) {
					textBox.Visibility = Visibility.Visible;
					textBox.Focus();
					textBox.SelectAll();
				}
			}
		}
		private void OnTextBlock_Mouse_Move(Object sender, MouseEventArgs e) {
			if (_captured) {
				var mouseX = e.GetPosition(this).X;
				var d = mouseX - _mouseXStart;
				if (Math.Abs(d) > SystemParameters.MinimumHorizontalDragDistance) {
					if (Keyboard.Modifiers.HasFlag(ModifierKeys.Control))
						_multiplier = 0.001;
					else if (Keyboard.Modifiers.HasFlag(ModifierKeys.Shift))
						_multiplier = 0.1;
					else
						_multiplier = 0.01;
						var newValue = _originalValue + (d * _multiplier * Multiplier);
					Value = newValue.ToString("0.#####");
					_valueChangedOnDrag = true;
				}
			}
		}

		static NumberBox() { // allows <Style TargetType="{x:Type NumberBox}"> in xaml files
			DefaultStyleKeyProperty.OverrideMetadata(
				typeof(NumberBox),
				new FrameworkPropertyMetadata(typeof(NumberBox))
			);
		}
	}
}
