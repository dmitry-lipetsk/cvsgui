from cvsgui.SafeTk import *
import string

"""
  CvsGui Macro Library Module "SubstModeTk"
  $Revision: 1.3 $
  
  written by Oliver Giesen, Oct 2004
  contact:
    email:  ogware@gmx.net
    jabber: ogiesen@jabber.org
    msn:    ogiesen@hotmail.com
    icq:    18777742

  Feel free to modify or distribute in whichever way you like,
   as long as it doesn't limit my personal rights to modify and
   redistribute this code.
   Apart from that the code is supplied "as-is", i.e. without warranty of any
   kind, either expressed or implied, regarding its quality or security.
   Have fun!

  =============
  Known Issues / "Un-niceties":

  - It is currently possible to select invalid combinations of flags,
   e.g. -kbo.

  Please report any problems you may encounter or suggestions you might have
  to ogware@gmx.net .
  
"""

if checkTk():
  class CvsSubstModeMenu(Menu):
    def __init__(self, master, oldflags, callback):
      Menu.__init__(self, master)
      self.callback = callback
      self.mode = StringVar()
      self.flags = {}

      oldenc = self.update(oldflags)
      self.createWidgets(oldenc)

    def update(self, text):
      enc = '{}'
      oldflags = text
      if len(oldflags) > 2:
        for m in ['t', 'b', 'B', 'u', enc]:
          if oldflags[2] == m[0]:
            if m == enc:
              closepos = oldflags.find('}')
              enc = oldflags[2:closepos+1]
              self.mode.set(enc)
              oldflags = oldflags[closepos+1:]
              if self.mnuCreated:
                self.entryconfigure('Custom encoding', value=enc)
            else:
              self.mode.set(m)
              oldflags = oldflags[3:]
            break
        else:
          self.mode.set(' ')
          oldflags = oldflags[2:]
      else:
        self.mode.set(' ')
        oldflags = ' '

      for f in ['o', 'k', 'v', 'l', 'c', 'z', 'L']:
        if not self.flags.has_key(f):
          self.flags[f] = StringVar()
        if f in oldflags:
          self.flags[f].set(f)
        else:
          self.flags[f].set(' ')
      return enc

    def createWidgets(self, oldenc):
      modes = [\
        ('Use server default (Text if no default defined)', ' '), \
        ('Text', 't'), \
        ('Binary', 'b'), \
        ('Binary w/ delta storage', 'B'), \
        ('Unicode', 'u'), \
        ('Custom encoding', oldenc)]
      for text, val in modes:
        if val == ' ':
          self.add_radiobutton(label=text, variable=self.mode, value=val, command=self.clear)
          self.add_separator()
        else:
          self.add_radiobutton(label=text, variable=self.mode, value=val, command=self.callback)

      self.add_separator()

      flaglist = [\
        ("Don't change keywords", 'o'),\
        ('Substitute keyword', 'k'),\
        ('Substitute value', 'v'),\
        ("Generate locker's name", 'l'),\
        ("Force reserved edit", 'c'),\
#        ("Force exclusive edit", 'x'),\
        ("Compress deltas", 'z'),\
        ("Force Unix LF", 'L')]
      
      for text, val in flaglist:
        self.add_checkbutton(label=text, variable=self.flags[val], onvalue=val, offvalue=' ', command=self.callback)
        if val in ['l', 'x']:
          self.add_separator()

      self.mnuCreated = 1          

    def clear(self):
      #self.mode.set(' ')
      for f in self.flags.values():
        f.set(' ')
      self.callback()
      

  class CvsSubstModeUI(Frame):
    def __init__(self, master, var):
      Frame.__init__(self, master)
      self.mnu = None
      self.var=var
      self.createWidgets()
      self.pack(fill=BOTH, expand=1)

    def get_mnu(self):
      if not self.mnu:
        self.mnu = CvsSubstModeMenu(self, self.var.get(), self.setFlags)
      return self.mnu  

    def popup(self, event=None):
      self.get_mnu()
      if event:
        self.mnu.post(event.x_root, event.y_root)
      else:
        self.mnu.post(0, 0)
      
    def createWidgets(self):
      self.entry = Entry(self, textvariable=self.var, width=10)
      self.entry.grid(row=0, column=1, sticky=W+E)
      self.entry.bind('<KeyRelease>', self.updateMnu)

      self.btn = Button(self, text='>')
      self.btn.grid(row=0, column=2)
      self.btn.bind('<Button-1>', self.popup)

      self.label = Label(self, width=25, anchor=W, justify=LEFT)
      self.label.grid(row=0, column=3, sticky=W+E)

    def updateMnu(self, event):
      self.get_mnu().update(self.var.get())
      flags = self.var.get()
      if '}' in flags:
        flags = flags[flags.find('}')+1:]
      else:
        flags = flags[2:]
      self.updateLabel(flags)
      
    def setFlags(self):
      newMode = self.get_mnu().mode.get().strip()
      flags = ''
      for f in self.mnu.flags.values():
        flags += f.get().strip()
      newMode += flags

      if len(newMode) > 0:
        newMode = '-k' + newMode

      self.var.set(newMode)

      self.updateLabel(flags)

    def updateLabel(self, flags):
      mode = self.get_mnu().mode.get().strip()
      lblText = ''
      if mode in ['b', 'B']:
        lblText = 'Binary'
      elif mode in ['u']:
        lblText = 'Unicode'
      elif mode.startswith('{'):
        lblText = 'Custom enc.'
      elif mode in ['t']:
        lblText = 'Text'
        
      if lblText == '':
        for c in flags:
          if c in ['k', 'v', 'l', 'o']:
            lblText = 'Text'
            break

      if 'o' in flags:
        lblText += ', no KW expansion'

      if mode + flags == '':
        lblText = 'Server default'

      self.label.configure(text=lblText)
      self.btn.configure(relief=RAISED)

else: # Tk not available
  class CvsSubstModeUI:
    pass