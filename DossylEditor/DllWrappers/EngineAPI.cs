﻿using DossylEditor.Components;
using DossylEditor.EngineAPIStructs;
using System;
using System.Collections.Generic;
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
    class GameEntityDescriptor {
        public TransformComponent Transform = new TransformComponent();
    }
}

namespace DossylEditor.DllWrapper {
    static class EngineAPI {
        private const string _dllName = "EngineDll.dll";

        [DllImport(_dllName)]
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

            return CreateGameEntity(desc);
        }

        [DllImport(_dllName)]
        public static extern void RemoveGameEntity(int id);
        public static void RemoveGameEntity(GameEntity entity) {
            RemoveGameEntity(entity.EntityId);
        }
    }
}
