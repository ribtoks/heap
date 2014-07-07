using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Automation;
using System.Windows.Input;

namespace MyUiVerify
{
    public class MainWindowViewModel : ObservableObject
    {
        private TreeWalker m_TreeWalker;

        #region Properties

        public ReadOnlyCollection<TreeAutomationElementViewModel> RootItems { get; private set; }

        private TreeAutomationElementViewModel m_RootElement;
        public TreeAutomationElementViewModel RootElement
        {
            get { return m_RootElement; }
            set
            {
                if (m_RootElement != value)
                {
                    m_RootElement = value;
                    OnPropertyChanged(() => RootElement);
                }
            }
        }

        private TreeAutomationElementViewModel m_SelectedElement;
        public TreeAutomationElementViewModel SelectedElement
        {
            get { return m_SelectedElement; }
            set
            {
                if (m_SelectedElement != value)
                {
                    m_SelectedElement = value;
                    OnPropertyChanged(() => SelectedElement);
                }
            }
        }

        #endregion

        public MainWindowViewModel()
        {
            m_TreeWalker = TreeWalker.ControlViewWalker;
            RootElement = new TreeAutomationElementViewModel(AutomationElement.RootElement, null, m_TreeWalker, lazyLoad:true);
            SelectedElement = RootElement;
            RootItems = new ReadOnlyCollection<TreeAutomationElementViewModel>(new[] {RootElement});
        }
    }
}
