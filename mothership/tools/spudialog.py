# Copyright (c) 2001, Stanford University
# All rights reserved.
#
# See the file LICENSE.txt for information on redistributing this software.
#
# Authors:
#   Brian Paul


"""The SPUDialog class builds a dialog with a set of options specified by
the caller.  Options can be boolean, integer, float or string.
"""

from wxPython.wx import *


class SPUDialog(wxDialog):
	def __init__(self, parent, id, title, options=None):
		"""parent, id, and title are the standard wxDialog parameters.
		options is a list of tuples (name, description, type, count, default,
		mins, maxs) that describes the controls to put in the dialog.
		"""
		wxDialog.__init__(self, parent, id, title, pos=wxPoint(-1,-1),
						  style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
		
		id_OK = 1
		id_CANCEL = 2

		self._Controls = {}

		outerSizer = wxBoxSizer(wxVERTICAL)

		box = wxStaticBox(parent=self, id=-1, label=title)
		innerSizer = wxStaticBoxSizer(box, wxVERTICAL)
		outerSizer.Add(innerSizer, option=1, flag=wxALL|wxGROW, border=10)
		
		if options:
			i = 0
			for (name, description, type, count, default, mins, maxs) in options:
				controls = []
				if type == "BOOL":
					labString = description + " (" + name + ")"
					# XXX support [count] widgets?
					assert count == 1
					ctrl = wxCheckBox(parent=self, id=100+i, label=labString)
					innerSizer.Add(ctrl, flag=wxLEFT|wxRIGHT, border=4)
					ctrl.SetValue( default[0] )
					controls.append(ctrl)
				elif type == "INT":
					rowSizer = wxBoxSizer(wxHORIZONTAL)
					labString = description + ": "
					label = wxStaticText(parent=self, id=-1, label=labString)
					rowSizer.Add(label, flag=wxALIGN_CENTRE_VERTICAL)
					for j in range(0, count):
						if len(mins) > j:
							minValue = mins[j]
						else:
							minValue = 0
						if len(maxs) > j:
							maxValue = maxs[j]
						else:
							maxValue = 1000000
						ctrl = wxSpinCtrl(parent=self, id=100+i,
										  size=wxSize(70,25),
										  value=str(default[j]),
										  min=minValue, max=maxValue)
						rowSizer.Add(ctrl)
						controls.append(ctrl)
						i += 1
					innerSizer.Add(rowSizer, flag=wxALL, border=4)
				elif type == "FLOAT":
					rowSizer = wxBoxSizer(wxHORIZONTAL)
					labString = description + ": "
					label = wxStaticText(parent=self, id=-1, label=labString)
					rowSizer.Add(label, flag=wxALIGN_CENTRE_VERTICAL)
					for j in range(0, count):
						ctrl = wxTextCtrl(parent=self, id=100+i,
										  size=wxSize(70,25),
										  value=str(default[j]))
						rowSizer.Add(ctrl)
						controls.append(ctrl)
						i += 1
					innerSizer.Add(rowSizer, flag=wxALL, border=4)
				else:
					assert type == "STRING"
					rowSizer = wxBoxSizer(wxHORIZONTAL)
					labString = description + ": "
					label = wxStaticText(parent=self, id=-1, label=labString)
					rowSizer.Add(label, flag=wxALIGN_CENTRE_VERTICAL)
					# XXX support [count] widgets?
					assert count == 1
					ctrl = wxTextCtrl(parent=self, id=100+i, value=default[0])
					rowSizer.Add(ctrl, option=1, flag=wxEXPAND)
					innerSizer.Add(rowSizer, flag=wxALL|wxEXPAND, border=4)
					controls.append(ctrl)

				# Save this option
				value = default  # a vector
				self._Controls[name] = controls   # a vector

				i += 1

		# XXX still need to write the callbacks for each control

		rowSizer = wxGridSizer(rows=1, cols=2, vgap=4, hgap=20)
		self.OkButton = wxButton(parent=self, id=id_OK, label="OK")
		rowSizer.Add(self.OkButton, option=0, flag=wxALIGN_CENTER, border=10)
		self.CancelButton = wxButton(parent=self, id=id_CANCEL, label="Cancel")
		rowSizer.Add(self.CancelButton, option=0, flag=wxALIGN_CENTER, border=10)
		outerSizer.Add(rowSizer, option=0, flag=wxGROW|wxBOTTOM, border=10)
		EVT_BUTTON(self.OkButton, id_OK, self._onOK)
		EVT_BUTTON(self.CancelButton, id_CANCEL, self._onCancel)

		min = outerSizer.GetMinSize()
		min[0] = 500
		self.SetSizer(outerSizer)
		self.SetAutoLayout(true)
		self.SetSizeHints(minW=min[0], minH=min[1])
		self.SetSize(min)

	def _onOK(self, event):
		"""Called by OK button"""
		self.EndModal(1)

	def _onCancel(self, event):
		"""Called by Cancel button"""
		self.EndModal(0)

	def IsOption(self, name):
		"""Return true if name is a recognized option."""
		return name in self._Controls

	# name is an SPU option like bbox_line_width
	def SetValue(self, name, newValue):
		"""Set a control's value (a vector of values)"""
		assert name in self._Controls.keys()
		ctrls = self._Controls[name]
		count = len(ctrls)
		assert len(newValue) == count
		if isinstance(ctrls[0], wxSpinCtrl) or isinstance(ctrls[0], wxCheckBox):
			for i in range(count):
				ival = int(newValue[i])
				ctrls[i].SetValue(ival)

	# name is an SPU option like bbox_line_width
	def GetValue(self, name):
		"""Return current value (vector) of the named control"""
		assert name in self._Controls.keys()
		ctrls = self._Controls[name]
		result = []
		count = len(ctrls)
		for i in range(count):
			result.append(ctrls[i].GetValue())
		return result

	# Override the wxDialog.ShowModal() method
	def ShowModal(self):
		"""Display dialog and return 0 for cancel, 1 for OK."""
		# Save starting values
		values = {}
		for name in self._Controls.keys():
			ctrls = self._Controls[name]
			vals = []
			for i in range(len(ctrls)):
				vals.append(ctrls[i].GetValue())
			values[name] = vals
		# Show the dialog
		retVal = wxDialog.ShowModal(self)
		# Finish up
		if retVal == 0:
			# Cancelled, restore original values
			for name in values.keys():
				ctrls = self._Controls[name]
				for i in range(len(ctrls)):
					ctrls[i].SetValue(values[name][i])
		return retVal
