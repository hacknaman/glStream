# Copyright (c) 2001, Stanford University
# All rights reserved.
#
# See the file LICENSE.txt for information on redistributing this software.
#
# Authors:
#   Brian Paul

"""Functions for Chromium configuration templates"""

from wxPython.wx import *
import crtypes
import crutils
import intdialog
from tilesort_template import *


# Eventually we'll move these into separate files in a special directory.
# We've only done this for the tilesort template (tilesort_template.py) so far.

def __Create_Sortlast(parentWindow, mothership):
	"""Create a sort-last configuration"""
	# XXX need a widget for the hostnames???
	dialog = intdialog.IntDialog(NULL, id=-1,
								 title="Sort-last Template",
								 labels=["Number of application nodes:"],
								 defaultValues=[2], maxValue=10000)
	if dialog.ShowModal() == wxID_CANCEL:
		dialog.Destroy()
		return 0
	numClients = dialog.GetValue()
	hostname = "localhost"
	mothership.DeselectAllNodes()
	# Create the server/render node
	xPos = 300
	if 1:
		yPos = 5
	else:
		yPos = numClients * 60 / 2 - 20
	serverNode = crtypes.NetworkNode(host=hostname)
	serverNode.SetPosition(xPos, yPos)
	serverNode.Select()
	renderSPU = crutils.NewSPU("render")
	serverNode.AddSPU(renderSPU)
	mothership.AddNode(serverNode)
	# Create the client/app nodes
	xPos = 5
	yPos = 5
	if 1:
		# Make one N-count node
		appNode = crtypes.ApplicationNode(host=hostname)
		appNode.SetPosition(xPos, yPos)
		appNode.SetCount(numClients)
		appNode.Select()
		readbackSPU = crutils.NewSPU("readback")
		appNode.AddSPU(readbackSPU)
		packSPU = crutils.NewSPU("pack")
		appNode.AddSPU(packSPU)
		mothership.AddNode(appNode)
		packSPU.AddServer(serverNode)
	else:
		# Make N discreet nodes
		for i in range(0, numClients):
			appNode = crtypes.ApplicationNode(host=hostname)
			appNode.SetPosition(xPos, yPos)
			appNode.Select()
			readbackSPU = crutils.NewSPU("readback")
			appNode.AddSPU(readbackSPU)
			packSPU = crutils.NewSPU("pack")
			appNode.AddSPU(packSPU)
			mothership.AddNode(appNode)
			packSPU.AddServer(serverNode)
			yPos += 60
	dialog.Destroy()
	return 1

def __Is_Sortlast(mothership):
	# XXX temporary
	return 1

def __Edit_Sortlast(parentWindow, mothership):
	"""Edit parameters for a sort-last template"""
	pass

def __Read_Sortlast(mothership, file):
	pass

def __Write_Sortlast(mothership, file):
	pass

def __Create_BinarySwap(parentWindow, mothership):
	"""Create a binary-swap, sort-last configuration"""
	# XXX need an integer dialog here!!!!
	# XXX also, a widget for the hostnames???
	dialog = intdialog.IntDialog(NULL, id=-1,
								 title="Binary-swap Template",
								 labels=["Number of application nodes:"],
								 defaultValues=[2], maxValue=10000)
	if dialog.ShowModal() == wxID_CANCEL:
		dialog.Destroy()
		return 0
	numClients = dialog.GetValue()
	hostname = "localhost"
	mothership.DeselectAllNodes()
	# Create the server/render node
	xPos = 300
	if 1:
		yPos = 5
	else:
		yPos = numClients * 60 / 2 - 20
	serverNode = crtypes.NetworkNode(host=hostname)
	serverNode.SetPosition(xPos, yPos)
	serverNode.Select()
	renderSPU = crutils.NewSPU("render")
	serverNode.AddSPU(renderSPU)
	mothership.AddNode(serverNode)
	# Create the client/app nodes
	xPos = 5
	yPos = 5
	if 1:
		appNode = crtypes.ApplicationNode(host=hostname)
		appNode.SetPosition(xPos, yPos)
		appNode.SetCount(numClients)
		appNode.Select()
		readbackSPU = crutils.NewSPU("binaryswap")
		appNode.AddSPU(readbackSPU)
		packSPU = crutils.NewSPU("pack")
		appNode.AddSPU(packSPU)
		mothership.AddNode(appNode)
		packSPU.AddServer(serverNode)
	else:
		for i in range(0, numClients):
			appNode = crtypes.ApplicationNode(host=hostname)
			appNode.SetPosition(xPos, yPos)
			appNode.Select()
			readbackSPU = crutils.NewSPU("binaryswap")
			appNode.AddSPU(readbackSPU)
			packSPU = crutils.NewSPU("pack")
			appNode.AddSPU(packSPU)
			mothership.AddNode(appNode)
			packSPU.AddServer(serverNode)
			yPos += 60
	dialog.Destroy()
	return 1

def __Is_BinarySwap(mothership):
	# XXX temporary
	return 1

def __Edit_BinarySwap(parentWindow, mothership):
	"""Edit parameters for a Tilesort template"""
	pass

def __Read_BinarySwap(mothership, file):
	pass

def __Write_BinarySwap(mothership, file):
	pass


# XXX Eventually we'll scan a template directory to do build a list
# of templates dynamically.
# XXX also specify functions for editing options, saving, validation, etc.

__Templates = {
	"Tilesort"     : (Create_Tilesort,Is_Tilesort, Edit_Tilesort, Read_Tilesort, Write_Tilesort),
	"Sort-last"    : (__Create_Sortlast, __Is_Sortlast, __Edit_Sortlast, __Read_Sortlast, __Write_Sortlast),
	"Binary-swap"  : (__Create_BinarySwap, __Is_BinarySwap, __Edit_BinarySwap, __Read_BinarySwap, __Write_BinarySwap),
#	"Lightning-2"  : (__Create_Lightning2, __Edit_Lightning2)
}

def GetTemplateList():
	"""Return list of names of known templates."""
	return __Templates.keys()


def CreateTemplate(templateName, parentWindow, mothership):
	"""Create an instance of the named template."""
	assert templateName in __Templates.keys()
	create = __Templates[templateName][0]
	assert create
	if create(parentWindow, mothership):
		mothership.SetTemplateType(templateName)
	else:
		mothership.SetTemplateType("")


def ValidateTemplate(templateName, mothership):
	"""Determine if the mothership matches the given template type."""
	assert templateName in __Templates.keys()
	(create, validate, edit, read, write) = __Templates[templateName]
	assert validate
	return validate(mothership)


def EditTemplate(templateName, parentWindow, mothership):
	"""Edit a templatized configuration."""
	assert templateName in __Templates.keys()
	(create, validate, edit, read, write) = __Templates[templateName]
	assert validate
	assert edit
	if validate(mothership):
		edit(parentWindow, mothership)
	else:
		# Inform the user that there's a problem
		dialog = wxMessageDialog(parent=parentWindow,
						 message="This configuration doesn't appear to be a "
						 + templateName + " configuration.",
						 caption="Notice", style=wxOK)
		dialog.ShowModal()
		#mothership.SetTemplateType("")


def ReadTemplate(templateName, mothership, file):
	"""Read a templatized mothership config from the given file handle."""
	assert templateName in __Templates.keys()
	(create, validate, edit, read, write) = __Templates[templateName]
	assert validate(mothership)
	read(mothership, file)


def WriteTemplate(templateName, mothership, file):
	"""Write the mothership config as a template to given file handle."""
	assert templateName in __Templates.keys()
	(create, validate, edit, read, write) = __Templates[templateName]
	assert validate(mothership)
	write(mothership, file)
