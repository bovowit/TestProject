using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Net.Mail;
using System.Net;

using System.Configuration;


namespace NovaLib.NTools
{
    public class EmailManager
    {

        public static void Send(string to, string subject, string contents)
        {
            string sender = ConfigurationManager.AppSettings["SMTPSender"];                 // App.config 에서 읽기
            Send(sender, to, subject, contents);
        }

        public static void Send(string from, string to, string subject, string contents)
        {
            Send(from, to, subject, contents, null, null);
        }

        public static void Send(string from, string to, string subject, string contents, string cc, string bcc)
        {
            string smtpHost = ConfigurationManager.AppSettings["SMTPHost"];                 //"gpm.smtp.com";
            int smtpPort = Convert.ToInt32(ConfigurationManager.AppSettings["SMTPPort"]);   //25 or ????;

            //int smppPort2 = int.Parse(ConfigurationManager.AppSettings["SMTPPort"]);
            string smtpId = ConfigurationManager.AppSettings["SMTPID"];                     //"nova";
            string smtpPwd = ConfigurationManager.AppSettings["SMTPPwd"];                   //"xxxxxxx";

            if (string.IsNullOrEmpty(from))
                return;
            MailMessage mailMsg = new MailMessage();
            mailMsg.From = new MailAddress(from);
            mailMsg.To.Add(to);
            if (!String.IsNullOrEmpty(cc))  // 참조
                mailMsg.CC.Add(cc);
            if (!String.IsNullOrEmpty(bcc))
                mailMsg.Bcc.Add(bcc);       // 숨은참조
            mailMsg.Subject = subject;
            mailMsg.IsBodyHtml = true;
            mailMsg.Body = contents;
            mailMsg.Priority = MailPriority.Normal;

            SmtpClient smtpClient = new SmtpClient();
            smtpClient.EnableSsl = true;
            smtpClient.UseDefaultCredentials = false;
            smtpClient.DeliveryMethod = SmtpDeliveryMethod.Network;
            smtpClient.Credentials = new NetworkCredential(smtpId, smtpPwd);
            smtpClient.Host = smtpHost;
            smtpClient.Port = smtpPort;

            try
            {
                smtpClient.Send(mailMsg);
            }
            catch (SmtpException ex)
            {
                string exp = ex.Message;
            }
            catch (Exception ex)
            {
                string exp = ex.Message;
            }
        }

    }
}
