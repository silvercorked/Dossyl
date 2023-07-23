using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;

namespace DossylEditor.Components {
    interface IMSComponent { } // used in place of MSComponent to avoid specifying the generic type

    [DataContract]
    abstract class Component : ViewModelBase {
        [DataMember]
        public GameEntity Owner { get; private set; }

        public Component(GameEntity entity) {
            Debug.Assert(entity != null);
            Owner = entity;
        }
    }

    abstract class MSComponent<T> : ViewModelBase, IMSComponent where T : Component {

    }
}
