using System;
using System.Web;
using System.Collections;
using System.Web.Services;
using System.Web.Services.Protocols;
using System.Runtime.InteropServices;
using System.Text;

[WebService(Namespace = "http://tempuri.org/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
public class Service : System.Web.Services.WebService 
{
    [DllImport("HmacOtpDll.dll")]
    static extern UInt32 GenerateOTP(string pbSecret, UInt32 cbSecret, UInt64 qwCount, StringBuilder szOTP, UInt32 cchOTP);

    private const int c_OTPLength = 7;

    public Service () 
    {
    }        

    [WebMethod]
    public bool VerifyOtpCode(string username, string otpCode) 
    {
        bool fResult = false;
        
        OtpDSTableAdapters.UsersTableAdapter taUsers = new OtpDSTableAdapters.UsersTableAdapter();
        OtpDS.UsersDataTable dtUser = taUsers.GetUserByUsername(username);
        if (dtUser.Count > 0)
        {
            string secretCode = dtUser[0].SecretCode;
            ulong counter = (ulong)dtUser[0].Counter;

            for (int i = 0; i < 1000; i++)
            {
                StringBuilder otpToVerify = new StringBuilder(c_OTPLength);
                GenerateOTP(secretCode, (uint) secretCode.Length, counter, otpToVerify, c_OTPLength);
                if (otpToVerify.ToString() == otpCode)
                {
                    // Update counter in DB
                    counter ++;
                    taUsers.UpdateCounter((long)counter, username);

                    fResult = true;
                    break;
                }

                counter++;
            }
        }

        return fResult;
    }
    
}

