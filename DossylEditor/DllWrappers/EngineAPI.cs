using DossylEditor.Components;
using DossylEditor.EngineAPIStructs;
using DossylEditor.GameProject;
using DossylEditor.Utilities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace DossylEditor.EngineAPIStructs {
    [StructLayout(LayoutKind.Sequential)]
    class TransformComponent {
        public Vector3 Position;
        public Vector3 Rotation;
        public Vector3 Scale = new Vector3(1, 1, 1);
    }

	[StructLayout(LayoutKind.Sequential)]
	class ScriptComponent {
		public IntPtr ScriptCreator;
	}

	[StructLayout(LayoutKind.Sequential)]
    class GameEntityDescriptor {
        public TransformComponent Transform = new TransformComponent();
		public ScriptComponent Script = new ScriptComponent();
    }
}

namespace DossylEditor.DllWrapper {
	static class EngineAPI {
		private const string _engineDll = "EngineDll.dll";

		[DllImport(_engineDll, CharSet = CharSet.Ansi)]
		public static extern int LoadGameCodeDll(string dllPath);
		[DllImport(_engineDll)]
		public static extern int UnloadGameCodeDll();
		[DllImport(_engineDll)]
		public static extern IntPtr GetScriptCreator(string name); // don't actually call in editor but use it to pass creator funcs to engine
		[DllImport(_engineDll)]
		[return: MarshalAs(UnmanagedType.SafeArray)]
		public static extern string[] GetScriptNames();

		internal static class EntityAPI {
			[DllImport(_engineDll)]
			private static extern int CreateGameEntity(GameEntityDescriptor desc);
			public static int CreateGameEntity(GameEntity entity) {
				GameEntityDescriptor desc = new GameEntityDescriptor();

				// transform component
				{
					var c = entity.GetComponent<Transform>();
					desc.Transform.Position = c.Position;
					desc.Transform.Rotation = c.Rotation;
					desc.Transform.Scale = c.Scale;
				}
				// script component
				{
					// Note: here we check if the current project is not null. if null, game code dll hasn't been loaded yet.
					var c = entity.GetComponent<Script>();
					if (c != null && Project.Current != null) {
						if (Project.Current.AvailableScripts.Contains(c.Name)) {
							desc.Script.ScriptCreator = GetScriptCreator(c.Name);
						}
						else {
							Logger.Log(MessageType.Error, $"Unable to find script with name {c.Name}. Game entity will be created without script component!");
						}
					}
				}
				return CreateGameEntity(desc);
			}

			[DllImport(_engineDll)]
			public static extern void RemoveGameEntity(int id);
			public static void RemoveGameEntity(GameEntity entity) {
				RemoveGameEntity(entity.EntityId);
			}
		}
	}
}
