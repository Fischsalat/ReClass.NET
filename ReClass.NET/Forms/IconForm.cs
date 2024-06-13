using System.Windows.Forms;
using System.Drawing;

namespace ReClassNET.Forms
{
	public class IconForm : Form
	{
		public IconForm()
		{
			Icon = Properties.Resources.ReClassNet;
		}

		public void UpdateColors(Color Background, Color Foreground)
		{
			this.BackColor = Background;
			this.ForeColor = Foreground;
		}
	}
}
