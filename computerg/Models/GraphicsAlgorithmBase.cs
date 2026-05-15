using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Windows.Media;
using System.Runtime.CompilerServices;

namespace ComputerGraphicsDemo.Models
{
    public abstract class GraphicsAlgorithmBase : INotifyPropertyChanged
    {
        private string _name = string.Empty;
        private string _description = string.Empty;
        private bool _isSelected;

        public string Name
        {
            get => _name;
            set
            {
                _name = value;
                OnPropertyChanged();
            }
        }

        public string Description
        {
            get => _description;
            set
            {
                _description = value;
                OnPropertyChanged();
            }
        }

        public bool IsSelected
        {
            get => _isSelected;
            set
            {
                _isSelected = value;
                OnPropertyChanged();
            }
        }

        public ObservableCollection<AlgorithmProperty> Properties { get; set; } = new();

        public abstract void Render(DrawingContext context, double width, double height);

        public abstract string GetCode();

        public event PropertyChangedEventHandler? PropertyChanged;

        protected virtual void OnPropertyChanged([CallerMemberName] string? propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        protected void SubscribeToPropertyChanges()
        {
            foreach (var prop in Properties)
            {
                prop.PropertyChanged += (s, e) =>
                {
                    if (e.PropertyName == nameof(AlgorithmProperty.Value) || e.PropertyName == nameof(AlgorithmProperty.IsEnabled))
                    {
                        OnPropertyChanged(nameof(Properties));
                    }
                };
            }
        }

        protected T? GetPropertyValue<T>(string propertyName)
        {
            var prop = Properties.FirstOrDefault(p => p.Name == propertyName);
            if (prop != null && prop.IsEnabled && prop.Value != null)
            {
                return (T)Convert.ChangeType(prop.Value, typeof(T));
            }
            return default;
        }
    }
}