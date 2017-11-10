using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;


namespace SeanReport
{
    public partial class formReport : Form
    {
        ReportViewCtl viewCtl = new ReportViewCtl();

        public formReport()
        {
            InitializeComponent();


        }
 
        private void formReport_Load(object sender, EventArgs e)
        {


        }

        private void StripMenuPdfSave_Click(object sender, EventArgs e)
        {
            labelDateStamp.Text = DateTime.Now.ToString("dd/MM/yyyy");

            viewCtl.CreatePdf(this);
        }
    }
}
