using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

// winform 캡쳐 후 인쇄
using System.Drawing.Printing;
using iTextSharp;
using iTextSharp.text;
using iTextSharp.text.pdf;

using System.Xml;

using NovaLib;
using NovaLib.NTools;

namespace SeanReport
{
    public partial class formLogin : Form
    {
        private Button printButton = new Button();
        Bitmap memoryImage;

        //LogManager LOG = new LogManager();                // default log
        LogManager LOG = new LogManager("SUR_", null);      // file 명에 prefix, postfix 추가

        public formLogin()
        {
            InitializeComponent();

            // 프린팅
            printDocument1.PrintPage += new PrintPageEventHandler(printDocument1_PrintPage);

            // 메일 전송 테스트
            EmailManager.Send("bovogoog@gmail.com", "email manager test from jsp C#", "so good !!!");
        }

        // 로그인 폼 로딩때 사용자 정보 읽기..
        private void formLogin_Load(object sender, EventArgs e)
        {

        }

        // 차트 테스트
        private void button1_Click(object sender, EventArgs e)
        {
            this.chart1.Series["FFT"].Points.AddXY("0", 33);
            this.chart1.Series["FFT"].Points.AddXY("0.1", 21);
            this.chart1.Series["FFT"].Points.AddXY("0.1", 13);
            this.chart1.Series["FFT"].Points.AddXY("", 19);
            this.chart1.Series["FFT"].Points.AddXY("", 33);
            this.chart1.Series["FFT"].Points.AddXY("0.3", 21);
            this.chart1.Series["FFT"].Points.AddXY("0.3", 13);
            this.chart1.Series["FFT"].Points.AddXY("0.4", 19);
            this.chart1.Series["FFT"].Points.AddXY("0.4", 33);
            this.chart1.Series["FFT"].Points.AddXY("0.5", 21);
            this.chart1.Series["FFT"].Points.AddXY("0.5", 13);
            this.chart1.Series["FFT"].Points.AddXY("0.6", 19);
        }

        private void btnSavePDF_Click(object sender, EventArgs e)
        {
            CaptureScreen();
            // 프린터 출력
            // printDocument1.Print();

            // PDF로 저장
            SavePDF();
        }

        private void SavePDF()
        {
            //Document docu = new PdfDocument();

            Document doc = new Document(iTextSharp.text.PageSize.A4);

            // PdfWriter가 doc 내용을 simple.pdf 파일에 쓰도록 설정
            PdfWriter wr = PdfWriter.GetInstance(doc, new FileStream("image_save.pdf", FileMode.Create));

            // Document 열기
            doc.Open();

            // Document에 내용 쓰기
            doc.AddTitle("Simple PDF 생성 예제");
            doc.AddAuthor("novashin");
            doc.AddCreationDate();

            // 영문쓰기
            doc.Add(new Paragraph("English : How are you? "));

            //// 한글쓰기
            string batangttf = System.IO.Path.Combine(Environment.GetEnvironmentVariable("SystemRoot"), @"Fonts\HANBatangExt.ttf");
            BaseFont batangBase = BaseFont.CreateFont(batangttf, BaseFont.IDENTITY_H, BaseFont.EMBEDDED);
            var batang = new iTextSharp.text.Font(batangBase, 12F);

            doc.Add(new Paragraph("한글: 안녕하세요?", batang));

            //iTextSharp.text.Image imag;
            iTextSharp.text.Image imag = iTextSharp.text.Image.GetInstance(memoryImage, System.Drawing.Imaging.ImageFormat.Jpeg);
            doc.Add(imag);

            // Document 닫기
            doc.Close();
        }

        private void printDocument1_PrintPage(System.Object sender,
               System.Drawing.Printing.PrintPageEventArgs e)
        {
           e.Graphics.DrawImage(memoryImage, 0, 0);
        }


        private void CaptureScreen()
        {
            Graphics myGraphics = this.CreateGraphics();
            //System.Drawing.Rectangle rect = this.DisplayRectangle;
            Size s = this.Size;// new Size();
            //s.Width = rect.Width;
            //s.Height = rect.Height;
            memoryImage = new Bitmap(s.Width, s.Height, myGraphics);
            Graphics memoryGraphics = Graphics.FromImage(memoryImage);

            memoryGraphics.CopyFromScreen(this.Location.X, this.Location.Y, 0, 0, s);
        }

        private void CaptureControl()
        {
            Graphics myGraphics = chart1.CreateGraphics();
            Size s = chart1.Size;
            memoryImage = new Bitmap(s.Width, s.Height, myGraphics);
            Graphics memoryGraphics = Graphics.FromImage(memoryImage);

            Point pos = PointToScreen(chart1.Location);
            memoryGraphics.CopyFromScreen(pos.X, pos.Y, 0, 0, s);
        }

        private void readXml()
        {
            string url = @"C:/Users\nova/Desktop/SEAN/samplingdata.db.xml";
            //string url = @"C:/Users\nova/Desktop/SEAN/data1.xml";
            try
            {
                XmlDocument xml = new XmlDocument();

                xml.Load(url);

                XmlNodeList xnList = xml.SelectNodes("/UltraVisionDatabaseExport/IndicationInformationFields"); //접근할 노드

                foreach (XmlNode xn in xnList)
                {

                    string part1 = xn["FieldId"].InnerText;         //
                    string part2 = xn["Name"].InnerText;         //
                    string part3 = xn["FieldPosition"].InnerText;   //

                    if (part2.Equals("Bandwidth"))
                    {
                        richTextBox1.AppendText(part1 + " | " + part2 + " | " + part3 + "\n");
                        LOG.WriteLine(richTextBox1.Text);
                    }
                }

            }
            catch (XmlException ex)
            {
                MessageBox.Show("XML 구문 문제 발생\r\n" + ex);
            }
            catch (ArgumentException ex)
            {

                MessageBox.Show("XML 파라메터 발생\r\n" + ex);

            }

        }

        private void btnReadXml_Click(object sender, EventArgs e)
        {
            readXml();
        }

        private void backgroundWorker1_DoWork(object sender, DoWorkEventArgs e)
        {

        }

        private void btnPrintDialog_Click(object sender, EventArgs e)
        {
            PrintDialog pDlg = new PrintDialog();
            pDlg.ShowDialog();
        }

        private void btnPrintPreviewDlg_Click(object sender, EventArgs e)
        {
            //CaptureScreen();
            CaptureControl();

            PrintPreviewDialog pDlg = new PrintPreviewDialog();
            pDlg.Document = printDocument1;
            pDlg.ShowDialog();

            Document doc = new Document(iTextSharp.text.PageSize.A4);
            PdfWriter wr = PdfWriter.GetInstance(doc, new FileStream("image_save.pdf", FileMode.Create));
            doc.Open();
            doc.AddTitle("Simple PDF 생성 예제");
            doc.AddAuthor("novashin");
            doc.AddCreationDate();
            doc.Add(new Paragraph("English : 보고서 캡쳐"));
            iTextSharp.text.Image imag = iTextSharp.text.Image.GetInstance(memoryImage, System.Drawing.Imaging.ImageFormat.Png);
            imag.SetAbsolutePosition(150, 550); //이미지 좌표
            //imag.ScaleToFit(75, 25); //이미지 크기
            doc.Add(imag);

            iTextSharp.text.Rectangle rect = new iTextSharp.text.Rectangle(20, 20, 30, 50);
            rect.BorderColor = BaseColor.BLACK;
            rect.BackgroundColor = BaseColor.YELLOW;
            //Paragraph paraRect = new Paragraph(rect);
            doc.Add(rect);

            StringBuilder sb = new StringBuilder("This is My First PDF Document");
            sb.AppendLine();
            sb.AppendLine();
            sb.Append(getHtmlString());
            sb.AppendLine();
            sb.AppendFormat("오늘의 날짜는 {0} 입니다.", System.DateTime.Now.ToLongDateString());
            sb.AppendLine();
            Paragraph contents = new Paragraph(sb.ToString());
            doc.Add(contents);
            
            //BidiLine line = new BidiLine();
            //line.TrimLeft(0, 0);
            //line.TrimRight(100, 50);
            //Paragraph paraLine = new Paragraph(sb.ToString());
            //doc.Add(paraLine);

            doc.Close();

        }

        private string getHtmlString()
        {
            string html = "<br/>" +
                "<p>From here HTML</p>" +
                "<table border='1'>" +
                "<tr>" +
                " <td>num</td>" +
                " <td>subject</td>" +
                " <td>date</td>" +
                "</tr>" +
                "<tr>" +
                " <td colspan='3' align='center'>there is no contents not yet</td>" +
                "</tr>" +
                "</table>";
            return html;
        }

        private void buttonLoadReportForm_Click(object sender, EventArgs e)
        {
            formReport form = new formReport();
            form.Show();
        }

    }
}
