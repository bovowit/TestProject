namespace SeanReport
{
    partial class formLogin
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다. 
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마세요.
        /// </summary>
        private void InitializeComponent()
        {
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend1 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series1 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(formLogin));
            this.label1 = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.chart1 = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.btnSavePDF = new System.Windows.Forms.Button();
            this.printDocument1 = new System.Drawing.Printing.PrintDocument();
            this.richTextBox1 = new System.Windows.Forms.RichTextBox();
            this.btnReadXml = new System.Windows.Forms.Button();
            this.backgroundWorker1 = new System.ComponentModel.BackgroundWorker();
            this.printDialog = new System.Windows.Forms.PrintDialog();
            this.printPreviewControl = new System.Windows.Forms.PrintPreviewControl();
            this.printPreviewDialog = new System.Windows.Forms.PrintPreviewDialog();
            this.btnPrintDialog = new System.Windows.Forms.Button();
            this.btnPrintPreviewDlg = new System.Windows.Forms.Button();
            this.buttonLoadReportForm = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.chart1)).BeginInit();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(31, 29);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(53, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "파일선택";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(405, 24);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 1;
            this.button1.Text = "LoadChart";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // chart1
            // 
            chartArea1.Name = "ChartArea1";
            this.chart1.ChartAreas.Add(chartArea1);
            legend1.Name = "Legend1";
            this.chart1.Legends.Add(legend1);
            this.chart1.Location = new System.Drawing.Point(378, 70);
            this.chart1.Name = "chart1";
            series1.ChartArea = "ChartArea1";
            series1.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series1.Legend = "Legend1";
            series1.Name = "FFT";
            this.chart1.Series.Add(series1);
            this.chart1.Size = new System.Drawing.Size(240, 109);
            this.chart1.TabIndex = 2;
            this.chart1.Text = "chart1";
            // 
            // btnSavePDF
            // 
            this.btnSavePDF.Location = new System.Drawing.Point(33, 64);
            this.btnSavePDF.Name = "btnSavePDF";
            this.btnSavePDF.Size = new System.Drawing.Size(234, 23);
            this.btnSavePDF.TabIndex = 3;
            this.btnSavePDF.Text = "프린트 & PDF 저장";
            this.btnSavePDF.UseVisualStyleBackColor = true;
            this.btnSavePDF.Click += new System.EventHandler(this.btnSavePDF_Click);
            // 
            // richTextBox1
            // 
            this.richTextBox1.Location = new System.Drawing.Point(378, 245);
            this.richTextBox1.Name = "richTextBox1";
            this.richTextBox1.Size = new System.Drawing.Size(272, 133);
            this.richTextBox1.TabIndex = 4;
            this.richTextBox1.Text = "";
            // 
            // btnReadXml
            // 
            this.btnReadXml.Location = new System.Drawing.Point(378, 211);
            this.btnReadXml.Name = "btnReadXml";
            this.btnReadXml.Size = new System.Drawing.Size(95, 23);
            this.btnReadXml.TabIndex = 5;
            this.btnReadXml.Text = "XML 읽기";
            this.btnReadXml.UseVisualStyleBackColor = true;
            this.btnReadXml.Click += new System.EventHandler(this.btnReadXml_Click);
            // 
            // backgroundWorker1
            // 
            this.backgroundWorker1.DoWork += new System.ComponentModel.DoWorkEventHandler(this.backgroundWorker1_DoWork);
            // 
            // printDialog
            // 
            this.printDialog.UseEXDialog = true;
            // 
            // printPreviewControl
            // 
            this.printPreviewControl.Location = new System.Drawing.Point(33, 251);
            this.printPreviewControl.Name = "printPreviewControl";
            this.printPreviewControl.Size = new System.Drawing.Size(279, 277);
            this.printPreviewControl.TabIndex = 6;
            // 
            // printPreviewDialog
            // 
            this.printPreviewDialog.AutoScrollMargin = new System.Drawing.Size(0, 0);
            this.printPreviewDialog.AutoScrollMinSize = new System.Drawing.Size(0, 0);
            this.printPreviewDialog.ClientSize = new System.Drawing.Size(400, 300);
            this.printPreviewDialog.Enabled = true;
            this.printPreviewDialog.Icon = ((System.Drawing.Icon)(resources.GetObject("printPreviewDialog.Icon")));
            this.printPreviewDialog.Name = "printPreviewDialog";
            this.printPreviewDialog.Visible = false;
            // 
            // btnPrintDialog
            // 
            this.btnPrintDialog.Location = new System.Drawing.Point(33, 147);
            this.btnPrintDialog.Name = "btnPrintDialog";
            this.btnPrintDialog.Size = new System.Drawing.Size(75, 23);
            this.btnPrintDialog.TabIndex = 7;
            this.btnPrintDialog.Text = "PrintDialog";
            this.btnPrintDialog.UseVisualStyleBackColor = true;
            this.btnPrintDialog.Click += new System.EventHandler(this.btnPrintDialog_Click);
            // 
            // btnPrintPreviewDlg
            // 
            this.btnPrintPreviewDlg.Location = new System.Drawing.Point(145, 150);
            this.btnPrintPreviewDlg.Name = "btnPrintPreviewDlg";
            this.btnPrintPreviewDlg.Size = new System.Drawing.Size(135, 23);
            this.btnPrintPreviewDlg.TabIndex = 8;
            this.btnPrintPreviewDlg.Text = "PrintPreviewDlg";
            this.btnPrintPreviewDlg.UseVisualStyleBackColor = true;
            this.btnPrintPreviewDlg.Click += new System.EventHandler(this.btnPrintPreviewDlg_Click);
            // 
            // buttonLoadReportForm
            // 
            this.buttonLoadReportForm.Location = new System.Drawing.Point(378, 442);
            this.buttonLoadReportForm.Name = "buttonLoadReportForm";
            this.buttonLoadReportForm.Size = new System.Drawing.Size(177, 23);
            this.buttonLoadReportForm.TabIndex = 9;
            this.buttonLoadReportForm.Text = "Load Report Form";
            this.buttonLoadReportForm.UseVisualStyleBackColor = true;
            this.buttonLoadReportForm.Click += new System.EventHandler(this.buttonLoadReportForm_Click);
            // 
            // formLogin
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.ClientSize = new System.Drawing.Size(704, 608);
            this.Controls.Add(this.buttonLoadReportForm);
            this.Controls.Add(this.btnPrintPreviewDlg);
            this.Controls.Add(this.btnPrintDialog);
            this.Controls.Add(this.printPreviewControl);
            this.Controls.Add(this.btnReadXml);
            this.Controls.Add(this.richTextBox1);
            this.Controls.Add(this.btnSavePDF);
            this.Controls.Add(this.chart1);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.label1);
            this.Name = "formLogin";
            this.Text = "사용자 설정";
            this.Load += new System.EventHandler(this.formLogin_Load);
            ((System.ComponentModel.ISupportInitialize)(this.chart1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.DataVisualization.Charting.Chart chart1;
        private System.Windows.Forms.Button btnSavePDF;
        private System.Drawing.Printing.PrintDocument printDocument1;
        private System.Windows.Forms.RichTextBox richTextBox1;
        private System.Windows.Forms.Button btnReadXml;
        private System.ComponentModel.BackgroundWorker backgroundWorker1;
        private System.Windows.Forms.PrintDialog printDialog;
        private System.Windows.Forms.PrintPreviewControl printPreviewControl;
        private System.Windows.Forms.PrintPreviewDialog printPreviewDialog;
        private System.Windows.Forms.Button btnPrintDialog;
        private System.Windows.Forms.Button btnPrintPreviewDlg;
        private System.Windows.Forms.Button buttonLoadReportForm;
    }
}

