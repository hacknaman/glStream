import os
arch = "WIN_NT"

crdir = "C:/Project/TransViz"

try:  
   os.environ["TransVizInstalledPath"]
   crdir = os.environ["TransVizInstalledPath"]
except KeyError: 
   print "Defaulting TransVizInstalledPath's Path "
   
crbindir = os.path.join(crdir,'bin/')
crlibdir = os.path.join(crdir,'lib/')

