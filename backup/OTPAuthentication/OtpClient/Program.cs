using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.IO;
using System.Xml;

namespace OtpClient
{
    class Program
    {
        [DllImport("HmacOtpDll.dll")]
        static extern UInt32 GenerateOTP(string pbSecret, UInt32 cbSecret, UInt64 qwCount, StringBuilder szOTP, UInt32 cchOTP);

        private const int c_OTPLength = 7;

        private static string XmlFilePath
        {
            get
            {
                System.Reflection.Assembly assembly = typeof(Program).Assembly;
                string directoryName = Path.GetDirectoryName(assembly.Location);
                string xmlFilePath = directoryName + "\\otp.xml";
                return xmlFilePath;
            }
        }

        static void Main(string[] args)
        {
            // Get secret code and counter from xml file
            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.Load(XmlFilePath);
            string secretKey = xmlDoc["otp"]["secretCode"].InnerText;
            ulong counter = ulong.Parse(xmlDoc["otp"]["counter"].InnerText);            

            // Generate otp
            StringBuilder otp = new StringBuilder(c_OTPLength);
            GenerateOTP(secretKey, (uint)secretKey.Length, (ulong)counter, otp, c_OTPLength);
            Console.WriteLine(otp);

            // Increment counter and save it back to the xml
            counter++;
            xmlDoc["otp"]["counter"].InnerText = counter.ToString();
            xmlDoc.Save(XmlFilePath);
        }
    }
}
