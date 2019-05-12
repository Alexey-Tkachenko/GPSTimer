namespace GPSTimerClient
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.mnuSaveToFile = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuReadAgain = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuShowMap = new System.Windows.Forms.ToolStripMenuItem();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.pTable = new System.Windows.Forms.Panel();
            this.dgTable = new System.Windows.Forms.DataGridView();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.tsDebugOut = new System.Windows.Forms.ToolStripStatusLabel();
            this.dlgSave = new System.Windows.Forms.SaveFileDialog();
            this.menuStrip1.SuspendLayout();
            this.pTable.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dgTable)).BeginInit();
            this.statusStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuSaveToFile,
            this.mnuReadAgain,
            this.mnuShowMap});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(678, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // mnuSaveToFile
            // 
            this.mnuSaveToFile.Name = "mnuSaveToFile";
            this.mnuSaveToFile.Size = new System.Drawing.Size(76, 20);
            this.mnuSaveToFile.Text = "Save to file";
            this.mnuSaveToFile.Click += new System.EventHandler(this.mnuSaveToFile_Click);
            // 
            // mnuReadAgain
            // 
            this.mnuReadAgain.Name = "mnuReadAgain";
            this.mnuReadAgain.Size = new System.Drawing.Size(77, 20);
            this.mnuReadAgain.Text = "Read again";
            this.mnuReadAgain.Visible = false;
            this.mnuReadAgain.Click += new System.EventHandler(this.mnuReadAgain_Click);
            // 
            // mnuShowMap
            // 
            this.mnuShowMap.Name = "mnuShowMap";
            this.mnuShowMap.Size = new System.Drawing.Size(75, 20);
            this.mnuShowMap.Text = "Show map";
            this.mnuShowMap.Click += new System.EventHandler(this.mnuShowMap_Click);
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(61, 4);
            // 
            // pTable
            // 
            this.pTable.Controls.Add(this.dgTable);
            this.pTable.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pTable.Location = new System.Drawing.Point(0, 24);
            this.pTable.Name = "pTable";
            this.pTable.Size = new System.Drawing.Size(678, 313);
            this.pTable.TabIndex = 2;
            // 
            // dgTable
            // 
            this.dgTable.AllowUserToAddRows = false;
            this.dgTable.AllowUserToDeleteRows = false;
            this.dgTable.AllowUserToOrderColumns = true;
            this.dgTable.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dgTable.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dgTable.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
            this.dgTable.Location = new System.Drawing.Point(0, 0);
            this.dgTable.Name = "dgTable";
            this.dgTable.ReadOnly = true;
            this.dgTable.RowHeadersVisible = false;
            this.dgTable.Size = new System.Drawing.Size(678, 313);
            this.dgTable.TabIndex = 0;
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsDebugOut});
            this.statusStrip1.Location = new System.Drawing.Point(0, 337);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(678, 22);
            this.statusStrip1.TabIndex = 4;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // tsDebugOut
            // 
            this.tsDebugOut.Name = "tsDebugOut";
            this.tsDebugOut.Size = new System.Drawing.Size(0, 17);
            // 
            // dlgSave
            // 
            this.dlgSave.DefaultExt = "txt";
            this.dlgSave.Filter = "Text files (*.txt)|*.txt|All files (*.*)|*.*";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(678, 359);
            this.Controls.Add(this.pTable);
            this.Controls.Add(this.menuStrip1);
            this.Controls.Add(this.statusStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "MainForm";
            this.Text = "GPSTimer client application";
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.pTable.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dgTable)).EndInit();
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem mnuReadAgain;
        private System.Windows.Forms.ToolStripMenuItem mnuShowMap;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.Panel pTable;
        private System.Windows.Forms.DataGridView dgTable;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel tsDebugOut;
        private System.Windows.Forms.ToolStripMenuItem mnuSaveToFile;
        private System.Windows.Forms.SaveFileDialog dlgSave;
    }
}

