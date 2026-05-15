
using System;
using System.Collections.Generic;
using System.CodeDom.Compiler;
using Microsoft.CSharp;
using System.Reflection;
using System.Drawing;

namespace GraphicsDemo
{
    public class CodeParser
    {
        private string code;
        private GraphicsPanel graphicsPanel;

        public CodeParser(string code, GraphicsPanel panel)
        {
            this.code = code;
            this.graphicsPanel = panel;
        }

        public void ParseAndExecute()
        {
            string fullCode = GenerateFullCode();
            
            CSharpCodeProvider provider = new CSharpCodeProvider();
            CompilerParameters parameters = new CompilerParameters();
            
            parameters.ReferencedAssemblies.Add("System.dll");
            parameters.ReferencedAssemblies.Add("System.Drawing.dll");
            parameters.ReferencedAssemblies.Add("System.Windows.Forms.dll");
            parameters.ReferencedAssemblies.Add(Assembly.GetExecutingAssembly().Location);
            parameters.GenerateInMemory = true;
            parameters.GenerateExecutable = false;
            
            CompilerResults results = provider.CompileAssemblyFromSource(parameters, fullCode);
            
            if (results.Errors.HasErrors)
            {
                string errors = "";
                foreach (CompilerError error in results.Errors)
                {
                    errors += $"错误 {error.Line}: {error.ErrorText}\n";
                }
                throw new Exception(errors);
            }
            
            Assembly assembly = results.CompiledAssembly;
            Type type = assembly.GetType("GraphicsDemo.DynamicRenderer");
            MethodInfo method = type.GetMethod("Render");
            
            if (method != null)
            {
                object instance = Activator.CreateInstance(type);
                method.Invoke(instance, new object[] { graphicsPanel });
            }
        }

        private string GenerateFullCode()
        {
            return @"
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

namespace GraphicsDemo
{
    public class DynamicRenderer
    {
        public void Render(GraphicsPanel panel)
        {
            panel.ClearCanvas();
            
            " + code + @"
            
            panel.CanvasBitmap.SetPixel(0, 0, panel.CanvasBitmap.GetPixel(0, 0));
        }
    }
}";
        }
    }
}
