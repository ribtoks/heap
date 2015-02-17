using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StringSwitchTest
{
    public enum BoobsColor { Green, Yellow, Dark}

    public class MyClass
    {
        public int NumberOfBoobs { get; set; }
        public BoobsColor ColorOfBoobs { get; set; }
        public string TypeOfBoobs { get; set; }
        public string ProcessedTypeOfBoobs { get; set; }
        public double DiameterOfBoobs { get; set; }
    }

    class Program
    {
        public const string LargeBoobsType = "Large Boobs";
        public const string ExtremeBoobsType = "Extreme Boobs";
        public const string UnknownBoobsType = "Unknown";
        public const string UserDefinedBoobs = "User Boobs";
        public const string PrivatePersonCleanBoobs = "Personal Person Mine Boobs";
        public const string FixedAcceptSSSBoobs = "Fixed Accept SSS";

        public static readonly string[] Constants =
        {
            LargeBoobsType, ExtremeBoobsType, UnknownBoobsType, UserDefinedBoobs,
            PrivatePersonCleanBoobs, FixedAcceptSSSBoobs
        };

        private static readonly IDictionary<String, String> BoobsTypeNameMap =
            new Dictionary<String, String>
            {
                {"MultipleType", "MultipleType"},

                {LargeBoobsType, "test 1"},
                {ExtremeBoobsType, "another unexpected"},
                {UserDefinedBoobs, "numbers"},
                {PrivatePersonCleanBoobs, "12345567"},
                {FixedAcceptSSSBoobs, ";jhibkjkgyufyj"},
            };

        public static String MapStringByDict(String type)
        {
            string result;
            if (type == null || !BoobsTypeNameMap.TryGetValue(type, out result))
            {
                result = "default value here";
            }

            return result;
        }

        private static string MapStringBySwitch(string line)
        {
            switch (line)
            {
                case LargeBoobsType:
                    return "test 1";
                case ExtremeBoobsType:
                    return "another unexpected";
                case UserDefinedBoobs:
                    return "numbers";
                case PrivatePersonCleanBoobs:
                    return "12345567";
                case FixedAcceptSSSBoobs:
                    return ";jhibkjkgyufyj";
                default:
                    return "default value here";
            }
        }

        private static void ProcessAllItemsBySwitch(List<MyClass> items)
        {
            items.ForEach(item => item.ProcessedTypeOfBoobs = MapStringBySwitch(item.TypeOfBoobs));
        }

        private static void ProcessAllItemsByDict(List<MyClass> items)
        {
            items.ForEach(item => item.ProcessedTypeOfBoobs = MapStringByDict(item.TypeOfBoobs));
        }

        static void Main(string[] args)
        {
            int tons = 10000000;
            var items = GenerageTonsOfItems(tons);

            // ensure methods are jit-byte-compiled
            var item1 = MapStringByDict("item2");
            var item2 = MapStringBySwitch("item1");
            var list1 = new List<MyClass>() {new MyClass() {TypeOfBoobs = "my type of boobs"}};
            var list2 = new List<MyClass>() {new MyClass() {TypeOfBoobs = "his type of boobs"}};
            ProcessAllItemsByDict(list1);
            ProcessAllItemsBySwitch(list2);

            Stopwatch watch;

            watch = Stopwatch.StartNew();
            ProcessAllItemsByDict(items); // current results are Debug: ~750 / Release: ~650
            watch.Stop();
            Console.WriteLine("Processing by dict took {0}", watch.ElapsedMilliseconds);
            
            watch = Stopwatch.StartNew();
            ProcessAllItemsBySwitch(items); // current results are Debug: ~450 / Release: 250
            watch.Stop();
            Console.WriteLine("Processing by switch took {0}", watch.ElapsedMilliseconds);

            // to ensure no optimizations were done to remove item1 and item2 
            // and do not byte-compile method first
            Random rand = new Random(DateTime.Now.Millisecond);
            if (rand.Next() % 2 == 0)
            {
                Console.WriteLine(item1);
                Console.WriteLine(list2.First().TypeOfBoobs);
            }
            else
            {
                Console.WriteLine(item2);
                Console.WriteLine(list1.First().TypeOfBoobs);
            }
        }

        private static List<MyClass> GenerageTonsOfItems(int tonsNumber)
        {
            Random rand = new Random(DateTime.Now.Millisecond);
            var items = new List<MyClass>(tonsNumber);
            
            for (int i = 0; i < tonsNumber; ++i)
            {
                var item = new MyClass() {
                    ColorOfBoobs = (BoobsColor)(i % 3), 
                    DiameterOfBoobs = 2.0, 
                    NumberOfBoobs = i,
                    TypeOfBoobs = Constants[rand.Next(Constants.Length)]};

                if (i%1235 == 0)
                {
                    item.TypeOfBoobs = null;
                }
                
                items.Add(item);
            }

            return items;
        } 
    }
}
