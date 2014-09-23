using System;
using System.Web;
using System.Web.Security;
using System.IO;

public class OtpModule : IHttpModule
{
    public OtpModule()
    {
    }    

    public void Init(HttpApplication application)
    {
        application.BeginRequest += new EventHandler(application_BeginRequest);
    }

    private bool IsAuthenticationPost(HttpApplication application)
    {
        bool fResult = false;
        HttpContext context = application.Context;

        if (context.Request.RequestType == "POST" && context.Request.Form["hdLoginForm"] == "1")
        {
            fResult = true;
        }

        return fResult;
    }

    private void TryAuthenticate(HttpApplication application)
    {
        HttpContext context = application.Context;

        string username = context.Request.Form["tbUsername"];
        string otpCode = context.Request.Form["tbOtpCode"];


        IIS7Module.OtpService.Service webService = new IIS7Module.OtpService.Service();        
        if (webService.VerifyOtpCode(username, otpCode))
        {
            FormsAuthenticationTicket ticket = new FormsAuthenticationTicket(username, true, 30);
            string encTicket = FormsAuthentication.Encrypt(ticket);
            context.Response.Cookies.Add(new HttpCookie(FormsAuthentication.FormsCookieName, encTicket));            
            context.Response.Redirect("/");
        }
        else
        {
            ShowLoginForm(application, "Username or otp code is wrong.");
        }
    }

    private void ShowLoginForm(HttpApplication application, string errorMsg)
    {
        HttpContext context = application.Context;

        string loginPageHTML = IIS7Module.HTMLRes.LoginPage;
        loginPageHTML = loginPageHTML.Replace("@ErrorMsg@", errorMsg);

        context.Response.Write(loginPageHTML);
        context.Response.End();
        application.CompleteRequest();
    }

    private bool IsAuthenticated(HttpApplication application)
    {
        bool fResult = false;
        HttpContext context = application.Context;

        HttpCookie authCookie = context.Request.Cookies[FormsAuthentication.FormsCookieName];
        if (authCookie != null)
        {
            FormsAuthenticationTicket ticket = FormsAuthentication.Decrypt(authCookie.Value);
            if (ticket != null)
            {
                // For demo purposes, set the authenticated user name
                FormsIdentity identity = new FormsIdentity(ticket);
                System.Security.Principal.GenericPrincipal principal = new System.Security.Principal.GenericPrincipal(identity, null);
                context.User = principal;
                fResult = true;
            }
        }

        return fResult;
    }

    void application_BeginRequest(object sender, EventArgs e)
    {
        HttpApplication application = (HttpApplication)sender;

        if (!IsAuthenticated(application))
        {
            if (IsAuthenticationPost(application))
            {
                TryAuthenticate(application);
            }
            else
            {
                ShowLoginForm(application, "");
            }
        }
    }

    public void Dispose() { }
}