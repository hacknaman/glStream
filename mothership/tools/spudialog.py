"""The SPUDialog class builds a dialog with a set of options specified by
the caller.  Options can be boolean, integer, float or string.
"""

from wxPython.wx import *


class SPUDialog(wxDialog):
	def __init__(self, parent, id, title, options=None):
		"""parent, id, and title are the standard wxDialog parameters.
		options is a list of tuples (name, type, count, default, description)
		that describes the controls to put in the dialog.
		"""
		wxDialog.__init__(self, parent, id, title, pos=wxPoint(-1,-1),
						  style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
		
		id_OK = 1
		id_CANCEL = 2

		outerSizer = wxBoxSizer(wxVERTICAL)

		box = wxStaticBox(parent=self, id=-1, label=title)
		innerSizer = wxStaticBoxSizer(box, wxVERTICAL)
		outerSizer.Add(innerSizer, option=1, flag=wxALL|wxGROW, border=10)
		
		if options:
			i = 0
			for (name, type, count, default, description) in options:
				if type == "bool":
					labString = description + " (" + name + ")"
					ctrl = wxCheckBox(parent=self, id=100+i, label=labString)
					innerSizer.Add(ctrl, flag=wxLEFT|wxRIGHT, border=4)
					ctrl.SetValue( default )
				elif type == "int":
					rowSizer = wxBoxSizer(wxHORIZONTAL)
					label = wxStaticText(parent=self, id=-1, label=description)
					rowSizer.Add(label, flag=wxALIGN_CENTRE_VERTICAL)
					ctrl = wxSpinCtrl(parent=self, id=100+i, value=str(default))
					rowSizer.Add(ctrl)
					innerSizer.Add(rowSizer, flag=wxALL, border=4)
				elif type == "float":
					rowSizer = wxBoxSizer(wxHORIZONTAL)
					labString = description + ": "
					label = wxStaticText(parent=self, id=-1, label=labString)
					rowSizer.Add(label, flag=wxALIGN_CENTRE_VERTICAL)
					ctrl = wxTextCtrl(parent=self, id=100+i, value=str(default))
					rowSizer.Add(ctrl)
					innerSizer.Add(rowSizer, flag=wxALL, border=4)
				elif type == "string":
					rowSizer = wxBoxSizer(wxHORIZONTAL)
					labString = description + ": "
					label = wxStaticText(parent=self, id=-1, label=labString)
					rowSizer.Add(label, flag=wxALIGN_CENTRE_VERTICAL)
					ctrl = wxTextCtrl(parent=self, id=100+i, value=default)
					rowSizer.Add(ctrl, option=1, flag=wxEXPAND)
					innerSizer.Add(rowSizer, flag=wxALL|wxEXPAND, border=4)
				i += 1

		# XXX still need to write the callbacks for each control

		rowSizer = wxGridSizer(rows=1, cols=2, vgap=4, hgap=20)
		self.OkButton = wxButton(parent=self, id=id_OK, label="OK")
		rowSizer.Add(self.OkButton, option=0, flag=wxALIGN_CENTER, border=10)
		self.CancelButton = wxButton(parent=self, id=id_CANCEL, label="Cancel")
		rowSizer.Add(self.CancelButton, option=0, flag=wxALIGN_CENTER, border=10)
		outerSizer.Add(rowSizer, option=0, flag=wxGROW|wxBOTTOM, border=10)
		EVT_BUTTON(self.OkButton, id_OK, self.onOK)
		EVT_BUTTON(self.CancelButton, id_CANCEL, self.onCancel)

		min = outerSizer.GetMinSize()
		min[0] = 500
		self.SetSizer(outerSizer)
		self.SetAutoLayout(true)
		self.SetSizeHints(minW=min[0], minH=min[1])
		self.SetSize(min)


	def onOK(self, event):
		self.EndModal(1)

	def onCancel(self, event):
		self.EndModal(0)
