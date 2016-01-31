using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Drawing;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Automation;
using System.Windows.Threading;

namespace MyUiVerify
{
    public class TreeAutomationElementViewModel : ObservableObject
    {
        private AutomationElement m_SourceElement;
        private TreeWalker m_TreeWalker;
        private bool m_IsDummy = false;

        #region Properties

        private string m_ElementName;
        public string ElementName
        {
            get { return m_ElementName; }
            set
            {
                if (m_ElementName != value)
                {
                    m_ElementName = value;
                    OnPropertyChanged(() => ElementName);
                }
            }
        }

        private readonly ObservableCollection<TreeAutomationElementViewModel> m_ChildrenElements =
            new ObservableCollection<TreeAutomationElementViewModel>();

        public ObservableCollection<TreeAutomationElementViewModel> ChildrenElements
        {
            get { return m_ChildrenElements; }
        }

        private TreeAutomationElementViewModel m_ParentElement;
        public TreeAutomationElementViewModel ParentElement
        {
            get { return m_ParentElement; }
            set
            {
                if (m_ParentElement != value)
                {
                    m_ParentElement = value;
                    OnPropertyChanged(() => ParentElement);
                }
            }
        }

        private bool m_IsSelected;
        public bool IsSelected
        {
            get { return m_IsSelected; }
            set
            {
                if (m_IsSelected != value)
                {
                    m_IsSelected = value;
                    OnPropertyChanged(() => IsSelected);
                }

                if (m_IsSelected)
                {
                    HighlightElement();
                }
            }
        }

        private bool m_IsExpanded;
        public bool IsExpanded
        {
            get { return m_IsExpanded; }
            set
            {
                if (m_IsExpanded != value)
                {
                    m_IsExpanded = value;
                    OnPropertyChanged(() => IsExpanded);
                }

                // Expand all the way up to the root.
                if (m_IsExpanded && m_ParentElement != null)
                {
                    m_ParentElement.IsExpanded = true;
                }

                if (this.HasDummyChild)
                {
                    RefreshChildren();
                }
            }
        }

        private Dictionary<string, string> m_PropertiesDict;
        public Dictionary<string, string> PropertiesDict
        {
            get { return m_PropertiesDict; }
            set
            {
                if (m_PropertiesDict != value)
                {
                    m_PropertiesDict = value;
                    OnPropertyChanged(() => PropertiesDict);
                }
            }
        }

        public bool IsWidthChangable
        {
            get
            {
                bool result = false;

                try
                {
                    var pattern = m_SourceElement.GetCurrentPattern(TransformPattern.Pattern);
                    var transformPattern = pattern as TransformPattern;
                    result = transformPattern != null && transformPattern.Current.CanResize;
                }
                catch { }
                
                return result;
            }
        }

        public double ElementWidth
        {
            get
            {
                double width = 0;
                try
                {
                    var rect = m_SourceElement.Current.BoundingRectangle;
                    width = rect.Width;
                }
                catch { }

                return width;
            }

            set
            {
                double width = 0, height = 0;
                
                try
                {
                    var rect = m_SourceElement.Current.BoundingRectangle;
                    width = rect.Width;
                    height = rect.Height;
                }
                catch 
                {
                    return;
                }

                if (Math.Abs(width - value) > 0.0001)
                {
                    var transformPattern = m_SourceElement.GetCurrentPattern(TransformPattern.Pattern) as TransformPattern;
                    if (transformPattern != null && transformPattern.Current.CanResize)
                    {
                        transformPattern.Resize(value, height);
                        OnPropertyChanged(() => ElementWidth);
                    }
                }                
            }
        }

        #endregion

        public TreeAutomationElementViewModel(AutomationElement sourceElement, TreeAutomationElementViewModel parentElement, TreeWalker treeWalker, bool lazyLoad)
        {
            if (sourceElement == null)
            {
                throw new ArgumentNullException("sourceElement");
            }

            if (treeWalker == null)
            {
                throw new ArgumentNullException("treeWalker");
            }

            m_SourceElement = sourceElement;
            m_ParentElement = parentElement;
            m_TreeWalker = treeWalker;

            ElementName = string.Format("\"{0}\" [{1}]", sourceElement.Current.Name, sourceElement.Current.ControlType.ProgrammaticName);
            DoRefreshNodeProperties();

            if (lazyLoad)
            {
                m_ChildrenElements.Add(GetDummyChild());
            }
        }

        private TreeAutomationElementViewModel(TreeAutomationElementViewModel parentElement)
        {
            m_ParentElement = parentElement;
            ElementName = "Empty";
            PropertiesDict = new Dictionary<string, string>();
            m_IsDummy = true;
        }

        private TreeAutomationElementViewModel GetDummyChild()
        {
            return new TreeAutomationElementViewModel(this);
        }

        private bool HasDummyChild
        {
            get { return this.ChildrenElements.Count == 1 && this.ChildrenElements[0].m_IsDummy; }
        }

        public void RefreshChildren()
        {
            if (ChildrenElements.Count > 0)
            {
                ChildrenElements.Clear();
            }

            Task.Factory.StartNew(DoRefreshChildren);
        }

        private void DoRefreshChildren()
        {
            var currentNode = m_TreeWalker.GetFirstChild(m_SourceElement);
            while (currentNode != null)
            {
                var item = new TreeAutomationElementViewModel(currentNode, this, m_TreeWalker, lazyLoad: true);
                Application.Current.Dispatcher.Invoke(DispatcherPriority.Normal,
                    new Action(() => this.ChildrenElements.Add(item)));

                currentNode = m_TreeWalker.GetNextSibling(currentNode);
            }
        }

        public void RefreshElementProperties()
        {
            if (m_PropertiesDict != null && m_PropertiesDict.Count > 0)
            {
                m_PropertiesDict.Clear();
            }

            DoRefreshNodeProperties();
        }

        private void DoRefreshNodeProperties()
        {
            var propertiesDict = new Dictionary<string, string>();
            var currentNode = m_SourceElement.Current;

            AddKeySafe(propertiesDict, "ClassName", () => currentNode.ClassName);
            AddKeySafe(propertiesDict, "ControlType", () => currentNode.ControlType.ProgrammaticName);
            AddKeySafe(propertiesDict, "AutomationId", () => currentNode.AutomationId);
            AddKeySafe(propertiesDict, "ProcessId", () => currentNode.ProcessId.ToString(CultureInfo.InvariantCulture));
            AddKeySafe(propertiesDict, "TopLeft", () => currentNode.BoundingRectangle.TopLeft.ToString());
            AddKeySafe(propertiesDict, "BottomRight", () => currentNode.BoundingRectangle.BottomRight.ToString());

            PropertiesDict = propertiesDict;
        }

        private void AddKeySafe(Dictionary<string, string> dict, string key, Func<string> getter) 
        {
            string result;

            try
            {
                result = getter();
                dict.Add(key, result);
            }
            catch { }
        }

        private void HighlightElement()
        {
            Rect rectangle = Rect.Empty;
                
            rectangle = (Rect) m_SourceElement.GetCurrentPropertyValue(AutomationElement.BoundingRectangleProperty, true);
            
            if (rectangle != Rect.Empty)
            {
                //if we have rectangle then set the highlighting rectangle
                var boundsRectangle = new Rectangle((int) rectangle.Left, (int) rectangle.Top, (int) rectangle.Width,
                    (int) rectangle.Height);
                ScreenElementHighlighter.HighlightArea(boundsRectangle);
            }
        }
    }
}
