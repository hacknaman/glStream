# Copyright (c) 2001, Stanford University
# All rights reserved.
#
# See the file LICENSE.txt for information on redistributing this software.
#
# Authors:
#   Brian Paul

""" Lightning2.py

    Tool for making Chromium configurations for Lightning-2

	Basically this means that we're going to have several tiles hosted
	on each rendering node and we'll reassemble the tiles into a coherent
	display using the Lightning2 hardware (or software).

"""

import string, cPickle, os.path, re
from wxPython.wx import *
import traceback, types
from spudialog import *
from crutils import *


#----------------------------------------------------------------------------
#                            System Constants
#----------------------------------------------------------------------------

# Our menu item IDs:

menu_TILESORT      = 100
menu_RENDER        = 101
menu_SERVER        = 102
menu_GLOBAL        = 103
menu_ABOUT         = 104

# Widget IDs
id_MuralWidth  = 3000
id_MuralHeight = 3001
id_TileChoice  = 3002
id_TileWidth   = 3003
id_TileHeight  = 3004
id_Layout      = 3005
id_HostText    = 3007
id_HostStart   = 3008
id_HostCount   = 3009

# Size of the drawing page, in pixels.
PAGE_WIDTH  = 1000
PAGE_HEIGHT = 1000

CommonTileSizes = [ [128, 128],
					[256, 256],
					[512, 512],
					[1024, 1024],
					[1280, 1024],
					[1600, 1200] ]

BackgroundColor = wxColor(90, 150, 190)

ServerColors = [
	wxColor(255, 50, 50),   # red
	wxColor(50, 255, 50),   # green
	wxColor(150, 150, 255), # blue
	wxColor(0, 200, 200),   # cyan
	wxColor(200, 0, 200),   # purple
	wxColor(255, 255, 0),   # yellow
	wxColor(255, 100, 0),   # orange
	wxColor(255, 150, 130), # pink
	wxColor(139, 115, 85),  # burlywood4
	wxColor(152, 251, 152), # pale green
	wxColor(205, 92, 92),   # indian red
	wxColor(102, 139, 139), # PaleTurquoise4
	wxColor(255, 236, 139), # light goldenrod
	wxColor(159, 154, 98),  # khaki4
	wxColor(149, 81, 147),  # orchid4
	wxColor(0, 139, 69),    # SpringGreen4
]
	
WindowTitle = "Chromium Lightning-2 Configuration"

WildcardPattern = "Chromium Configs (*.conf)|*.conf|All (*)|*"

ServerOptions = [
	("optimize_bucket", "Optimized Extent Bucketing", "BOOL", 1, [1], [], []),
	("lighting2", "Generate Lightning-2 Strip Headers", "BOOL", 1, [0], [], [])
]

GlobalOptions = [
	("minimum_window_size", "Minimum Chromium App Window Size (w h)", "INT", 2, [0, 0], [0, 0], []),
	("match_window_title", "Match App Window Title", "STRING", 1, [""], [], []),
	("show_cursor", "Show Virtual cursor", "BOOL", 1, [0], [], []),
	("MTU", "Mean Transmission Unit (bytes)", "INT", 1, [1024*1024], [0], []),
	("default_app", "Default Application Program", "STRING", 1, [""], [], []),
	("auto_start", "Automatically Start Servers", "BOOL", 1, [0], [], [])
]

# This is the guts of the configuration script.
# It's simply appended to the file after we write all the configuration options
ConfigBody = """
import string
import sys
sys.path.append( "../server" )
sys.path.append( "../tools" )
from mothership import *
from crutils import *

# Get program name
if len(sys.argv) == 1:
	program = GLOBAL_default_app
elif len(sys.argv) == 2:
	program = sys.argv[1]
else:
	print "Usage: %s <program>" % sys.argv[0] 
	sys.exit(-1)
if program == "":
	print "No program to run!"
	sys.exit(-1)

# Determine if tiles are on one server or many
if string.find(HOSTNAME, '#') == -1:
	singleServer = 1
else:
	singleServer = 0

localHostname = os.uname()[1]

cr = CR()
cr.MTU( GLOBAL_MTU )

tilesortspu = SPU('tilesort')
tilesortspu.Conf('broadcast', TILESORT_broadcast)
tilesortspu.Conf('optimize_bucket', TILESORT_optimize_bucket)
tilesortspu.Conf('sync_on_swap', TILESORT_sync_on_swap)
tilesortspu.Conf('sync_on_finish', TILESORT_sync_on_finish)
tilesortspu.Conf('draw_bbox', TILESORT_draw_bbox)
tilesortspu.Conf('bbox_line_width', TILESORT_bbox_line_width)
#tilesortspu.Conf('fake_window_dims', fixme)
tilesortspu.Conf('scale_to_mural_size', TILESORT_scale_to_mural_size)


clientnode = CRApplicationNode()
clientnode.AddSPU(tilesortspu)

clientnode.StartDir( crbindir )
clientnode.SetApplication( os.path.join(crbindir, program) )
if GLOBAL_auto_start:
	clientnode.AutoStart( ["/bin/sh", "-c",
		"LD_LIBRARY_PATH=%s /usr/local/bin/crappfaker" % crlibdir] )


for row in range(TILE_ROWS):
	for col in range(TILE_COLS):

		# layout directions
		if RIGHT_TO_LEFT:
			j = TILE_COLS - col - 1
		else:
			j = col
		if BOTTOM_TO_TOP:
			i = TILE_ROWS - row - 1
		else:
			i = row

		# compute index for this tile
		index = i * TILE_COLS + j

		renderspu = SPU('render')
		renderspu.Conf('try_direct', RENDER_try_direct)
		renderspu.Conf('force_direct', RENDER_force_direct)
		renderspu.Conf('fullscreen', RENDER_fullscreen)
		renderspu.Conf('title', RENDER_title)
		renderspu.Conf('system_gl_path', RENDER_system_gl_path)

		if singleServer:
			renderspu.Conf('window_geometry',
						   int(1.1 * col * TILE_WIDTH),
						   int(1.1 * row * TILE_HEIGHT),
						   TILE_WIDTH, TILE_HEIGHT)
			host = HOSTNAME
		else:
			renderspu.Conf('window_geometry', 0, 0, TILE_WIDTH, TILE_HEIGHT)
			host = MakeHostname(HOSTNAME, HOSTSTART + index)
		servernode = CRNetworkNode(host)

		servernode.AddTile(col * TILE_WIDTH,
						   (TILE_ROWS - row - 1) * TILE_HEIGHT,
						   TILE_WIDTH, TILE_HEIGHT)

		servernode.AddSPU(renderspu)
		servernode.Conf('optimize_bucket', SERVER_optimize_bucket)

		cr.AddNode(servernode)
		tilesortspu.AddServer(servernode, protocol='tcpip', port = 7000 + index)

		if GLOBAL_auto_start:
			servernode.AutoStart( ["/usr/bin/rsh", host,
									"/bin/sh -c 'DISPLAY=:0.0  CRMOTHERSHIP=%s  LD_LIBRARY_PATH=%s  crserver'" % (localHostname, crlibdir) ] )


cr.AddNode(clientnode)
cr.SetParam('minimum_window_size', GLOBAL_minimum_window_size)
cr.SetParam('match_window_title', GLOBAL_match_window_title)
cr.SetParam('show_cursor', GLOBAL_show_cursor)
cr.Go()

"""


#----------------------------------------------------------------------------

class MainFrame(wxFrame):
	""" A frame showing the contents of a single document. """

	# ==========================================
	# == Initialisation and Window Management ==
	# ==========================================

	def __init__(self, parent, id, title, fileName=None):
		""" Standard constructor.

			'parent', 'id' and 'title' are all passed to the standard wxFrame
			constructor.  'fileName' is the name and path of a saved file to
			load into this frame, if any.
		"""
		wxFrame.__init__(self, parent, id, title,
				 style = wxDEFAULT_FRAME_STYLE | wxWANTS_CHARS |
					 wxNO_FULL_REPAINT_ON_RESIZE)

		self.HostNamePattern = "host##"
		self.HostNameStart = 0
		self.HostNameCount = 4
		self.Tiles = []

		# Setup our menu bar.
		menuBar = wxMenuBar()

		# File menu
		self.fileMenu = wxMenu()
		self.fileMenu.Append(wxID_NEW,    "New\tCTRL-N")
		self.fileMenu.Append(wxID_OPEN,   "Open...\tCTRL-O")
		self.fileMenu.Append(wxID_CLOSE,  "Close\tCTRL-W")
		self.fileMenu.AppendSeparator()
		self.fileMenu.Append(wxID_SAVE,   "Save\tCTRL-S")
		self.fileMenu.Append(wxID_SAVEAS, "Save As...")
		self.fileMenu.Append(wxID_REVERT, "Revert...")
		self.fileMenu.AppendSeparator()
		self.fileMenu.Append(wxID_EXIT,   "Quit\tCTRL-Q")
		EVT_MENU(self, wxID_NEW,    self.doNew)
		EVT_MENU(self, wxID_OPEN,   self.doOpen)
		EVT_MENU(self, wxID_CLOSE,  self.doClose)
		EVT_MENU(self, wxID_SAVE,   self.doSave)
		EVT_MENU(self, wxID_SAVEAS, self.doSaveAs)
		EVT_MENU(self, wxID_REVERT, self.doRevert)
		EVT_MENU(self, wxID_EXIT,   self.doExit)
		menuBar.Append(self.fileMenu, "File")

		# Options menu
		self.optionsMenu = wxMenu()
		self.optionsMenu.Append(menu_TILESORT, "Tilesort SPU...")
		self.optionsMenu.Append(menu_RENDER,   "Render SPU...")
		self.optionsMenu.Append(menu_SERVER,   "Server...")
		self.optionsMenu.Append(menu_GLOBAL,   "Global...")
		EVT_MENU(self, menu_TILESORT,  self.doTilesort)
		EVT_MENU(self, menu_RENDER, self.doRender)
		EVT_MENU(self, menu_SERVER, self.doServer)
		EVT_MENU(self, menu_GLOBAL, self.doGlobal)
		menuBar.Append(self.optionsMenu, "Options")

		# Help menu
		self.helpMenu = wxMenu()
		self.helpMenu.Append(menu_ABOUT, "About Lightning-2 Config...")
		EVT_MENU(self, menu_ABOUT, self.doShowAbout)
		menuBar.Append(self.helpMenu, "Help")

		self.SetMenuBar(menuBar)

		# Install our own method to handle closing the window.  This allows us
		# to ask the user if he/she wants to save before closing the window, as
		# well as keeping track of which windows are currently open.

		EVT_CLOSE(self, self.doClose)

		# Setup our top-most panel.  This holds the entire contents of the
		# window, excluding the menu bar.

		self.topPanel = wxPanel(self, id=-1)

		toolSizer = wxBoxSizer(wxVERTICAL)

		# Host naming
		box = wxStaticBox(parent=self.topPanel, id=-1, label="Hosts",
						  style=wxDOUBLE_BORDER)
		hostSizer = wxStaticBoxSizer(box, wxVERTICAL)
		# XXX should probably use a wxComboBox here so we can keep a small
		# history of frequently used hostname pattern strings.

		flexSizer = wxFlexGridSizer(rows=2, cols=2, hgap=4, vgap=4)
		hostLabel = wxStaticText(parent=self.topPanel, id=-1, label="Base name:")
		self.hostText = wxTextCtrl(parent=self.topPanel, id=id_HostText,
								   value=self.HostNamePattern)
		EVT_TEXT(self.hostText, id_HostText, self.onHostChange)
		
		startLabel = wxStaticText(parent=self.topPanel, id=-1, label="First:")
		self.hostStart = wxSpinCtrl(parent=self.topPanel, id=id_HostStart,
								   value=str(self.HostNameStart), min=0)
		EVT_SPINCTRL(self.hostStart, id_HostStart, self.onHostChange)
		countLabel = wxStaticText(parent=self.topPanel, id=-1, label="Number:")
		self.hostCount = wxSpinCtrl(parent=self.topPanel, id=id_HostCount,
								   value=str(self.HostNameCount), min=1)
		EVT_SPINCTRL(self.hostCount, id_HostCount, self.onHostChange)

		flexSizer.Add(hostLabel, flag=wxALIGN_CENTER_VERTICAL)
		flexSizer.Add(self.hostText, flag=wxEXPAND)
		flexSizer.Add(startLabel, flag=wxALIGN_CENTER_VERTICAL)
		flexSizer.Add(self.hostStart)
		flexSizer.Add(countLabel, flag=wxALIGN_CENTER_VERTICAL)
		flexSizer.Add(self.hostCount)
		hostSizer.Add(flexSizer)

		toolSizer.Add(hostSizer, flag=wxEXPAND)


		# Mural width/height (in tiles)
		box = wxStaticBox(parent=self.topPanel, id=-1, label="Mural Size",
						  style=wxDOUBLE_BORDER)
		muralSizer = wxStaticBoxSizer(box, wxVERTICAL)
		flexSizer = wxFlexGridSizer(rows=2, cols=2, hgap=4, vgap=4)
		columnsLabel = wxStaticText(parent=self.topPanel, id=-1,
									label="Columns")
		self.widthControl = wxSpinCtrl(parent=self.topPanel, id=id_MuralWidth,
									   value="5", min=1, max=16,
									   size=wxSize(50,25))
		rowsLabel = wxStaticText(parent=self.topPanel, id=-1,
								 label="Rows:")
		self.heightControl = wxSpinCtrl(parent=self.topPanel,
										id=id_MuralHeight,
										value="4", min=1, max=16,
										size=wxSize(50,25))
		EVT_SPINCTRL(self.widthControl, id_MuralWidth, self.onSizeChange)
		EVT_SPINCTRL(self.heightControl, id_MuralHeight, self.onSizeChange)
		flexSizer.Add(columnsLabel, flag=wxALIGN_CENTER_VERTICAL)
		flexSizer.Add(self.widthControl)
		flexSizer.Add(rowsLabel, flag=wxALIGN_CENTER_VERTICAL)
		flexSizer.Add(self.heightControl)
		muralSizer.Add(flexSizer)
		toolSizer.Add(muralSizer, flag=wxEXPAND)

		# Tile size (in pixels)
		box = wxStaticBox(parent=self.topPanel, id=-1, label="Tile Size",
						  style=wxDOUBLE_BORDER)
		tileSizer = wxStaticBoxSizer(box, wxVERTICAL)
		flexSizer = wxFlexGridSizer(rows=2, cols=2, hgap=4, vgap=4)
		tileChoices = []
		for i in CommonTileSizes:
			tileChoices.append( str("%d x %d" % (i[0], i[1])) )
		tileChoices.append("Custom")
		self.tileChoice = wxChoice(parent=self.topPanel, id=id_TileChoice,
								   choices=tileChoices)
		flexSizer = wxFlexGridSizer(rows=2, cols=2, hgap=4, vgap=4)
		self.tileWidthLabel = wxStaticText(parent=self.topPanel, id=-1,
										   label="Width:")
		self.tileWidthControl = wxSpinCtrl(parent=self.topPanel,
										   id=id_TileWidth,
										   value="256", min=128, max=2048,
										   size=wxSize(80,25))
		self.tileHeightLabel = wxStaticText(parent=self.topPanel, id=-1,
											label="Height:")
		self.tileHeightControl = wxSpinCtrl(parent=self.topPanel,
											id=id_TileHeight,
											value="256", min=128, max=2048,
											size=wxSize(80,25))
		EVT_SPINCTRL(self.tileWidthControl, id_TileWidth, self.onSizeChange)
		EVT_SPINCTRL(self.tileHeightControl, id_TileHeight, self.onSizeChange)
		EVT_CHOICE(self.tileChoice, id_TileChoice, self.onTileChoice)
		flexSizer.Add(self.tileWidthLabel, flag=wxALIGN_CENTER_VERTICAL)
		flexSizer.Add(self.tileWidthControl)
		flexSizer.Add(self.tileHeightLabel, flag=wxALIGN_CENTER_VERTICAL)
		flexSizer.Add(self.tileHeightControl)
		tileSizer.Add(self.tileChoice, flag=wxALIGN_CENTER)
		tileSizer.Add(flexSizer)
		toolSizer.Add(tileSizer, flag=wxEXPAND)

		# Total mural size (in pixels)
		box = wxStaticBox(parent=self.topPanel, id=-1, label="Total Size",
						  style=wxDOUBLE_BORDER)
		totalSizer = wxStaticBoxSizer(box, wxVERTICAL)
		self.totalSizeLabel = wxStaticText(parent=self.topPanel, id=-1,
										   label="??")
		totalSizer.Add(self.totalSizeLabel, flag=wxEXPAND)
		toolSizer.Add(totalSizer, flag=wxEXPAND)

		layoutChoices = [ "Raster order", "Zig-zag Raster order",
						  "Spiral from center" ]
		self.layoutRadio = wxRadioBox(parent=self.topPanel, id=id_Layout,
									  label="Layout", choices=layoutChoices,
									  majorDimension=1,
									  style=wxRA_SPECIFY_COLS )
		toolSizer.Add(self.layoutRadio, flag=wxEXPAND)
		EVT_RADIOBOX(self.layoutRadio, id_Layout, self.onLayoutChange)

		# Setup the main drawing area.
#		self.drawArea = wxScrolledWindow(self.topPanel, -1,
#										 style=wxSUNKEN_BORDER)
#		self.drawArea.EnableScrolling(true, true)
#		self.drawArea.SetScrollbars(20, 20, PAGE_WIDTH / 20, PAGE_HEIGHT / 20)
		self.drawArea = wxPanel(self.topPanel, id=-1, style=wxSUNKEN_BORDER)
		self.drawArea.SetBackgroundColour(BackgroundColor)
		EVT_PAINT(self.drawArea, self.onPaintEvent)

		# Position everything in the window.
		topSizer = wxBoxSizer(wxHORIZONTAL)
		topSizer.Add(toolSizer, option=0,
					 flag=wxTOP | wxLEFT | wxRIGHT | wxALIGN_TOP, border=5)
		topSizer.Add(self.drawArea, 1, wxEXPAND)

		self.topPanel.SetAutoLayout(true)
		self.topPanel.SetSizer(topSizer)

		self.SetSizeHints(minW=250, minH=200)
		self.SetSize(wxSize(700, 450))

		self.dirty     = false
		self.fileName  = fileName

		self.RecomputeTotalSize()
		self.LayoutTiles()
		
		# Make the Tilesort SPU options dialog
		tilesortInfo = GetSPUOptions("tilesort")
		assert tilesortInfo
		(tilesortParams, tilesortOptions) = tilesortInfo
		self.TilesortDialog = SPUDialog(parent=NULL, id=-1,
										title="Tilesort SPU Options",
										options=tilesortOptions)

		# Make the render SPU options dialog
		renderInfo = GetSPUOptions("render")
		assert renderInfo
		(renderParams, renderOptions) = renderInfo
		self.RenderDialog = SPUDialog(parent=NULL, id=-1,
									  title="Render SPU Options",
									  options=renderOptions)

		# Make the server options dialog
		self.ServerDialog = SPUDialog(parent=NULL, id=-1,
									  title="Server Options",
									  options=ServerOptions)

		# Make the global options dialog
		self.GlobalDialog = SPUDialog(parent=NULL, id=-1,
									  title="Global Chromium Options",
									  options=GlobalOptions)

		if self.fileName != None:
			self.loadConfiguration()


	#----------------------------------------------------------------------
	# Helper functions

	# This is called whenever the mural width/height or tile width/height
	# changes.
	def RecomputeTotalSize(self):
		"""Recompute the total mural size in pixels and update the widgets."""
		tileW = self.tileWidthControl.GetValue()
		tileH = self.tileHeightControl.GetValue()
		totalW = self.widthControl.GetValue() * tileW
		totalH = self.heightControl.GetValue() * tileH
		self.totalSizeLabel.SetLabel(str("%d x %d" % (totalW, totalH)))
		custom = 1
		for i in range(0, len(CommonTileSizes)):
			if tileW == CommonTileSizes[i][0] and tileH == CommonTileSizes[i][1]:
				self.tileChoice.SetSelection(i)
				return
		# must be custom size
		self.tileChoice.SetSelection(len(CommonTileSizes))  # "Custom"

	def LayoutTiles(self):
		"""Compute location and host number for the tiles."""
		cols = self.widthControl.GetValue()
		rows = self.heightControl.GetValue()
		numServers = self.hostCount.GetValue()
		layoutOrder = self.layoutRadio.GetSelection()
		self.Tiles = []  # list of (row, tile, server) tuples
		if layoutOrder == 0:
			# Simple raster order layout
			for i in range(rows):
				for j in range(cols):
					server = (i * cols + j) % numServers
					self.Tiles.append((i, j, server))
		elif layoutOrder == 1:
			# Slightly different raster order layout
			for i in range(rows):
				for j in range(cols):
					if i % 2 == 1:
						# odd row
						server = (i * cols + (cols - j - 1)) % numServers
					else:
						# even row
						server = (i * cols + j) % numServers
					self.Tiles.append((i, j, server))
		else:
			# Spiral outward from the center (this is a little tricky)
			assert layoutOrder == 2
			curRow = (rows - 1) / 2
			curCol = (cols - 1) / 2
			radius = 0
			march = 0
			colStep = 0
			rowStep = -1
			serv = 0
			while 1:
				assert ((rowStep == 0 and colStep != 0) or
						(rowStep != 0 and colStep == 0))
				if (curRow >= 0 and curRow < rows and
					curCol >= 0 and	curCol < cols):
					# save this tile location
					#server = len(self.Tiles) % numServers
					server = serv % numServers
					assert (curRow, curCol, server) not in self.Tiles
					self.Tiles.append((curRow, curCol, server))
					# check if we're done
					if len(self.Tiles) >= rows * cols:
						# all done
						break
				serv += 1
				# advance to next space
				march += 1
				if march < radius:
					# step in current direction
					curRow += rowStep
					curCol += colStep
					pass
				else:
					# change direction
					if colStep == 1 and rowStep == 0:
						# transition right -> down
						colStep = 0
						rowStep = 1
					elif colStep == 0 and rowStep == 1:
						# transition down -> left
						colStep = -1
						rowStep = 0
						radius += 1
					elif colStep == -1 and rowStep == 0:
						# transition left -> up
						colStep = 0
						rowStep = -1
					else:
						# transition up -> right
						assert colStep == 0
						assert rowStep == -1
						colStep = 1
						rowStep = 0
						radius += 1
					#endif
					march = 0
					curRow += rowStep
					curCol += colStep
				#endif
			#endwhile
		#endif
	#enddef

		
	#----------------------------------------------------------------------
	# Widget callback functions

	def onSizeChange(self, event):
		"""Called when tile size changes with spin controls."""
		self.RecomputeTotalSize()
		self.LayoutTiles()
		self.drawArea.Refresh()
		self.dirty = true

	def onLayoutChange(self, event):
		"""Called when Layout order changes."""
		self.LayoutTiles()
		self.drawArea.Refresh()
		self.dirty = true

	def onTileChoice(self, event):
		"""Called when tile size changes with combo-box control."""
		i = self.tileChoice.GetSelection()
		if i < len(CommonTileSizes):
			w = CommonTileSizes[i][0]
			h = CommonTileSizes[i][1]
			self.tileWidthControl.SetValue(w)
			self.tileHeightControl.SetValue(h)
		self.RecomputeTotalSize()
		self.LayoutTiles()
		self.drawArea.Refresh()
		self.dirty = true

	# Called when hostname or first host index changes
	def onHostChange(self, event):
		"""Called when the host name pattern or first index changes."""
		self.HostNamePattern = self.hostText.GetValue()
		self.HostNameStart = self.hostStart.GetValue()
		self.HostNameCount = self.hostCount.GetValue()
		self.LayoutTiles()
		self.drawArea.Refresh()
		self.dirty = true


	def onPaintEvent(self, event):
		""" Respond to a request to redraw the contents of our drawing panel.
		"""
		dc = wxPaintDC(self.drawArea)
#		self.drawArea.PrepareDC(dc)  # only for scrolled windows
		dc.BeginDrawing()
		dc.SetPen(wxBLACK_PEN);
		dc.SetBrush(wxLIGHT_GREY_BRUSH);

		# border around the window and space between the tiles
		border = 20
		space = 2
		
		# Get current settings
		size = self.drawArea.GetSize()
		cols = self.widthControl.GetValue()
		rows = self.heightControl.GetValue()
		tileWidth = self.tileWidthControl.GetValue()
		tileHeight = self.tileHeightControl.GetValue()
		layout = self.layoutRadio.GetSelection()

		# how many pixels we'd like to draw
		desiredWidth = cols * tileWidth
		desiredHeight = rows * tileHeight

		# how much space we have in the window
		availWidth = size.width - 2 * border - (cols + 1) * space
		availHeight = size.height - 2 * border - (rows + 1) * space

		# Compute scale factor so we fill the drawing area with the
		# screen diagram, preserving the tile and screen aspect ratios.
		xscale = float(availWidth) / float(desiredWidth)
		yscale = float(availHeight) / float(desiredHeight)

		if xscale < yscale:
			scale = xscale
		else:
			scale = yscale

		w = tileWidth * scale
		h = tileHeight * scale

		# draw the tiles as boxes
		numColors = len(ServerColors)
		numServers = self.hostCount.GetValue()
		for (row, col, server) in self.Tiles:
			x = col * (w + space) + border
			y = row * (h + space) + border
			color = server % numColors
			dc.SetBrush(wxBrush(ServerColors[color]))
			dc.DrawRectangle(x, y, w,h)
			s = MakeHostname(self.HostNamePattern, self.HostNameStart + server)
			(tw, th) = dc.GetTextExtent(s)
			dx = (w - tw) / 2
			dy = (h - th) / 2
			dc.DrawText(s, x+dx, y+dy)
		dc.EndDrawing()

		#for i in range(rows):
		#	for j in range(cols):
		#		x = j * (w + space) + border
		#		y = i * (h + space) + border
		#		server = (i * cols + j) % numServers
		#		color = server % numColors
		#		dc.SetBrush(wxBrush(ServerColors[color]))
		#		dc.DrawRectangle(x, y, w,h)
		#		s = MakeHostname(self.HostNamePattern, self.HostNameStart + server)
		#		(tw, th) = dc.GetTextExtent(s)
		#		dx = (w - tw) / 2
		#		dy = (h - th) / 2
		#		dc.DrawText(s, x+dx, y+dy)
		dc.EndDrawing()

		# draw top width label
		topLabel = "<---  %d  --->" % desiredWidth
		(tw, th) = dc.GetTextExtent(topLabel)
		dc.DrawText(topLabel, (size.width - tw) / 2, 2)

		# draw left height label (very crude for now)
		leftLabel = "^|| %d ||v" % desiredHeight
		(tw, th) = dc.GetTextExtent(leftLabel[0:1])
		totalH = th * len(leftLabel)
		x = 6
		y = (size.height - totalH) / 2
		for i in range(0, len(leftLabel)):
			dc.DrawText(leftLabel[i:i+1], x, y)
			y += th

	# ==========================
	# == Menu Command Methods ==
	# ==========================

	def doNew(self, event):
		"""File / New callback"""
		global _docList
		newFrame = MainFrame(None, -1, WindowTitle)
		newFrame.Show(TRUE)
		_docList.append(newFrame)


	def doOpen(self, event):
		"""File / Open callback"""
		global _docList

		curDir = os.getcwd()
		fileName = wxFileSelector("Open File", default_extension="conf",
					  wildcard=WildcardPattern,
					  flags = wxOPEN | wxFILE_MUST_EXIST)
		if fileName == "": return
		fileName = os.path.join(os.getcwd(), fileName)
		os.chdir(curDir)

		winTitle = WindowTitle + ": " + os.path.basename(fileName)

		if (self.fileName == None) and not self.dirty:
			# Load contents into current (empty) document.
			self.fileName = fileName
			self.SetTitle(winTitle)
			self.loadConfiguration()
		else:
			# Open a new frame for this document.
			newFrame = MainFrame(None, -1, title=winTitle,
						fileName=fileName)
			newFrame.Show(true)
			_docList.append(newFrame)


	def doClose(self, event):
		"""File / Close callback"""
		global _docList

		if self.dirty:
			if not self.askIfUserWantsToSave("closing"):
				return

		_docList.remove(self)
		self.Destroy()


	def doSave(self, event):
		"""File / Save callback"""
		if self.fileName == None:
			self.doSaveAs(event)
		else:
			self.saveConfiguration()


	def doSaveAs(self, event):
		"""File / Save As callback """
		if self.fileName == None:
			default = ""
		else:
			default = self.fileName

		curDir = os.getcwd()
		fileName = wxFileSelector("Save Configuration As",
					  default_filename=default,
					  default_extension="conf",
					  wildcard=WildcardPattern,
					  flags = wxSAVE | wxOVERWRITE_PROMPT)
		if fileName == "":
			return # User cancelled.
		fileName = os.path.join(os.getcwd(), fileName)
		os.chdir(curDir)

		title = os.path.basename(fileName)
		self.SetTitle(WindowTitle + ": " + title)

		self.fileName = fileName
		self.saveConfiguration()


	def doRevert(self, event):
		"""File / Revert callback"""
		if not self.dirty:
			return

		if wxMessageBox("Discard changes made to this document?", "Confirm",
				style = wxOK | wxCANCEL | wxICON_QUESTION,
				parent=self) == wxCANCEL: return
		self.loadConfiguration()


	def doExit(self, event):
		"""File / Exit callback"""
		global _docList, _app
#		for doc in _docList:
#			if not doc.dirty:
#				continue
#			doc.Raise()
#			if not doc.askIfUserWantsToSave("quitting"):
#				return
#			_docList.remove(doc)
#			doc.Destroy()

		_app.ExitMainLoop()

	def doTilesort(self, event):
		"""Options / Tilesort menu callback"""
		v = self.TilesortDialog.ShowModal()
		return

	def doRender(self, event):
		"""Options / Render menu callback"""
		v = self.RenderDialog.ShowModal()
		return

	def doServer(self, event):
		"""Options / Serverer menu callback"""
		v = self.ServerDialog.ShowModal()
		return

	def doGlobal(self, event):
		"""Options / Global menu callback"""
		v = self.GlobalDialog.ShowModal()
		return

	def doShowAbout(self, event):
		"""Help / About menu callback"""
		dialog = wxDialog(self, -1, "About") # ,
				  #style=wxDIALOG_MODAL | wxSTAY_ON_TOP)
		dialog.SetBackgroundColour(wxWHITE)

		panel = wxPanel(dialog, -1)
		panel.SetBackgroundColour(wxWHITE)

		panelSizer = wxBoxSizer(wxVERTICAL)

		boldFont = wxFont(panel.GetFont().GetPointSize(),
				  panel.GetFont().GetFamily(),
				  wxNORMAL, wxBOLD)

		logo = wxStaticBitmap(panel, -1, wxBitmap("images/logo.bmp",
							  wxBITMAP_TYPE_BMP))

		lab1 = wxStaticText(panel, -1, "not done yet")
		lab1.SetFont(wxFont(12, boldFont.GetFamily(), wxITALIC, wxBOLD))
		lab1.SetSize(lab1.GetBestSize())

		imageSizer = wxBoxSizer(wxHORIZONTAL)
		imageSizer.Add(logo, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 5)
		imageSizer.Add(lab1, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 5)

		btnOK = wxButton(panel, wxID_OK, "OK")

		panelSizer.Add(imageSizer, 0, wxALIGN_CENTRE)
		panelSizer.Add(10, 10) # Spacer.
		panelSizer.Add(btnOK, 0, wxALL | wxALIGN_CENTRE, 5)

		panel.SetAutoLayout(true)
		panel.SetSizer(panelSizer)
		panelSizer.Fit(panel)

		topSizer = wxBoxSizer(wxHORIZONTAL)
		topSizer.Add(panel, 0, wxALL, 10)

		dialog.SetAutoLayout(true)
		dialog.SetSizer(topSizer)
		topSizer.Fit(dialog)

		dialog.Centre()

		btn = dialog.ShowModal()
		dialog.Destroy()

	def loadConfiguration(self):
		"""Load a configuration file."""
		f = open(self.fileName, "r")
		if f:
			while true:
				l = f.readline()
				if not l:
					break
				if re.match("^TILE_ROWS = [0-9]+$", l):
					v = re.search("[0-9]+", l)
					self.heightControl.SetValue(int(l[v.start() : v.end()]))
				elif re.match("^TILE_COLS = [0-9]+$", l):
					v = re.search("[0-9]+", l)
					self.widthControl.SetValue(int(l[v.start() : v.end()]))
				elif re.match("^TILE_WIDTH = [0-9]+$", l):
					v = re.search("[0-9]+", l)
					self.tileWidthControl.SetValue(int(l[v.start() : v.end()]))
				elif re.match("^TILE_HEIGHT = [0-9]+$", l):
					v = re.search("[0-9]+", l)
					self.tileHeightControl.SetValue(int(l[v.start() : v.end()]))
				elif re.match("^HOSTNAME = ", l):
					# look for string in quotes
					v = re.search("\".+\"", l)
					# extract the string
					self.HostNamePattern = l[v.start()+1 : v.end()-1]
					self.hostText.SetValue(self.HostNamePattern)
				elif re.match("^HOSTSTART = [0-9]+$", l):
					v = re.search("[0-9]+", l)
					self.HostNameStart = int(l[v.start() : v.end()])
					self.hostStart.SetValue(self.HostNameStart)
				elif re.match("^HOSTCOUNT = [0-9]+$", l):
					v = re.search("[0-9]+", l)
					self.HostNameCount = int(l[v.start() : v.end()])
					self.hostCount.SetValue(self.HostNameCount)
				elif re.match("^TILESORT_", l):
					# A tilesort SPU option
					# extract the option name and value
					# parentheses in the regexp define groups
					# \"? is an optional double-quote character
					# [^\"] is any character but double-quote
					v = re.search("^TILESORT_([a-zA-Z0-9\_]+) = \"?([^\"]*)\"?", l)
					if v:
						name = v.group(1)
						value = v.group(2)
						if self.TilesortDialog.IsOption(name):
							self.TilesortDialog.SetValue(name, value)
						else:
							print "%s is not a recognized tilesort SPU option" % name
				elif re.match("^RENDER_", l):
					# A render SPU option
					v = re.search("^RENDER_([a-zA-Z0-9\_]+) = \"?([^\"]*)\"?", l)
					if v:
						name = v.group(1)
						value = v.group(2)
						if self.RenderDialog.IsOption(name):
							self.RenderDialog.SetValue(name, value)
						else:
							print "%s is not a recognized render SPU option" % name
				elif re.match("^SERVER_", l):
					# A server option
					v = re.search("^SERVER_([a-zA-Z0-9\_]+) = \"?([^\"]*)\"?", l)
					if v:
						name = v.group(1)
						value = v.group(2)
						if self.ServerDialog.IsOption(name):
							self.ServerDialog.SetValue(name, value)
						else:
							print "%s is not a recognized server option" % name
				elif re.match("^GLOBAL_", l):
					# A global option
					v = re.search("^GLOBAL_([a-zA-Z0-9\_]+) = \"?([^\"]*)\"?", l)
					if v:
						name = v.group(1)
						value = v.group(2)
						if self.GlobalDialog.IsOption(name):
							self.GlobalDialog.SetValue(name, value)
						else:
							print "%s is not a recognized global option" % name
				elif re.match("^# end of options$", l):
					# that's the end of the variables
					# save the rest of the file....
					break
				elif not re.match("\s*#", l):
					print "unrecognized line: %s" % l
			f.close()
			self.RecomputeTotalSize()
		self.dirty = false
		self.drawArea.Refresh()

	def writeOptions(self, file, prefix, options, dialog):
		"""Helper function for writing config file options"""
		for (name, type, count, default, descrip) in options:
			if type == "int" or type == "bool":
				file.write("%s_%s = %d\n" % (prefix, name, int(dialog.GetValue(name))))
			elif type == "float":
				file.write("%s_%s = %f\n" % (prefix, name, float(dialog.GetValue(name))))
			else:
				file.write("%s_%s = \"%s\"\n" % (prefix, name, dialog.GetValue(name)))
		# endfor

	def saveConfiguration(self):
		"""Save the configuration."""
		f = open(self.fileName, "w")
		if f:
			f.write("# Chromium Lightning-2 config file\n")
			f.write("TILE_ROWS = %d\n" % self.heightControl.GetValue())
			f.write("TILE_COLS = %d\n" % self.widthControl.GetValue())
			f.write("TILE_WIDTH = %d\n" % self.tileWidthControl.GetValue())
			f.write("TILE_HEIGHT = %d\n" % self.tileHeightControl.GetValue())
			f.write("HOSTNAME = \"%s\"\n" % self.HostNamePattern)
			f.write("HOSTSTART = %d\n" % self.HostNameStart)
			self.writeOptions(f, "TILESORT", TilesortOptions, self.TilesortDialog)
			self.writeOptions(f, "RENDER", RenderOptions, self.RenderDialog)
			self.writeOptions(f, "SERVER", ServerOptions, self.ServerDialog)
			self.writeOptions(f, "GLOBAL", GlobalOptions, self.GlobalDialog)
			f.write("# end of options\n")
			f.write(ConfigBody)
			f.close()
		self.dirty = false


	def askIfUserWantsToSave(self, action):
		""" Give the user the opportunity to save the current document.

			'action' is a string describing the action about to be taken.  If
			the user wants to save the document, it is saved immediately.  If
			the user cancels, we return false.
		"""
		if not self.dirty: return true # Nothing to do.

		response = wxMessageBox("Save changes before " + action + "?",
					"Confirm", wxYES_NO | wxCANCEL, self)

		if response == wxYES:
			if self.fileName == None:
				fileName = wxFileSelector(message = "Save Configuration As",
										  default_extension="conf",
										  wildcard=WildcardPattern,
										  flags = wxSAVE | wxOVERWRITE_PROMPT)
				if fileName == "": return false # User cancelled.
				self.fileName = fileName

			self.saveConfiguration()
			return true
		elif response == wxNO:
			return true # User doesn't want changes saved.
		elif response == wxCANCEL:
			return false # User cancelled.


#----------------------------------------------------------------------------

class ExceptionHandler:
	""" A simple error-handling class to write exceptions to a text file.

	    Under MS Windows, the standard DOS console window doesn't scroll and
	    closes as soon as the application exits, making it hard to find and
	    view Python exceptions.  This utility class allows you to handle Python
	    exceptions in a more friendly manner.
	"""

	def __init__(self):
		""" Standard constructor.
		"""
		self._buff = ""
		if os.path.exists("errors.txt"):
			os.remove("errors.txt") # Delete previous error log, if any.


	def write(self, s):
		""" Write the given error message to a text file.

			Note that if the error message doesn't end in a carriage return, we
			have to buffer up the inputs until a carriage return is received.
		"""
		if (s[-1] != "\n") and (s[-1] != "\r"):
			self._buff = self._buff + s
			return

		try:
			s = self._buff + s
			self._buff = ""

			if s[:9] == "Traceback":
			# Tell the user than an exception occurred.
				wxMessageBox("An internal error has occurred.\nPlease " + \
					 "refer to the 'errors.txt' file for details.",
					 "Error", wxOK | wxCENTRE | wxICON_EXCLAMATION)

			f = open("errors.txt", "a")
			f.write(s)
			f.close()
		except:
			pass # Don't recursively crash on errors.

#----------------------------------------------------------------------------

class LightningApp(wxApp):
	""" The main application class.
	"""
	def OnInit(self):
		""" Initialise the application.
		"""
		wxInitAllImageHandlers()

		global _docList
		_docList = []

		if len(sys.argv) == 1:
			# No file name was specified on the command line -> start with a
			# blank document.
			frame = MainFrame(None, -1, WindowTitle)
			frame.Centre()
			frame.Show(TRUE)
			_docList.append(frame)
		else:
			# Load the file(s) specified on the command line.
			for arg in sys.argv[1:]:
				fileName = os.path.join(os.getcwd(), arg)
				if os.path.isfile(fileName):
					winTitle = WindowTitle + ": " + os.path.basename(fileName)
					frame = MainFrame(None, -1,
						 title=winTitle,
						 fileName=fileName)
					frame.Show(TRUE)
					_docList.append(frame)

		return TRUE

#----------------------------------------------------------------------------

def main():
	""" Start up the application.
	"""
	global _app

	# Redirect python exceptions to a log file.

	sys.stderr = ExceptionHandler()

	# Create and start the Tilesort application.

	_app = LightningApp(0)
	_app.MainLoop()


if __name__ == "__main__":
	main()

