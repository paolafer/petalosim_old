### ------------------------------------------------------------------
###  SConstruct - Building script for petalosim
###
###  Note.- The first time this script is executed, a configure-like step
###  tries to find out where the header and libraries
###  of all petalosim dependencies are located. If it succeeds,
###  this configuration is stored in a file and used in future builds
###  until a clean is performed. The script tries to locate the dependencies
###  by using pkg-config scripts or searching at common paths. This should work
###  in most systems. However, if needed, users can provide via
###  command-line (or the BUILDVARS_FILE) the system path to any
###  dependency.
###
### The PETALO Collaboration
### ------------------------------------------------------------------

import os
import subprocess

## Geant4 version required by petalosim
MIN_G4VERSION_NUMBER = 1100

## petalosim source code directories
SRCDIR = ['actions',
          'generators',
          'geometries',
          'materials',
          'persistency',
          'physics',
          'physics_lists',
          'sensdet',
          'utils']

## File where the build variables are stored
BUILDVARS_FILE = 'buildvars.scons'

## Dummy default for path-like variables
NULL_PATH = '/dev/null'

## Some useful functions

def Abort(message):
    """Outputs a message before exiting with an error."""
    print ('scons: Build aborted.')
    print ('scons: {}'.format(message))
    Exit(1)

def AssertG4Version(path):

    ## If a path was specified, add it to the cmd to be executed
    cmd = "geant4-config --version"
    if path != NULL_PATH:
        cmd = path + '/' + cmd

    ## Execute the command and store the stdout and stderr
    p = subprocess.Popen(cmd, shell=True,
                         stdout=subprocess.PIPE,
                         stderr=subprocess.PIPE)

    ## If the stdout is empty, the execution of the command failed
    version = p.stdout.read()
    if version == b'':
        Abort("Failed to establish Geant4 version.")
    else:
        g4version = version.decode('utf-8').strip()
        g4version = int(''.join(g4version.split('.')))
        msg = "Checking for Geant4 version..."
        print (msg, g4version)
        if g4version < MIN_G4VERSION_NUMBER:
            msg = 'This version of petalosim requires Geant4 version >= ' \
                + str(MIN_G4VERSION_NUMBER)
            Abort(msg)



## ###################################################################
## BUILD VARIABLES
## These variables can be used to specify (via command-line or file)
## the location of dependencies. They are also used to store in file
## configured values of compilation flags ($CPPPATH, $LIBS, etc.).

## Create a Variables instance associated to  a file
vars = Variables(BUILDVARS_FILE)

## Definition of the variables
vars.AddVariables(

    ## Geant4

    PathVariable('GEANT4_BINDIR',                 # var name
                 'Path to Geant4 bin directory.',  # var description
                 NULL_PATH),                      # var default value

    ## ROOT

    PathVariable('ROOT_BINDIR',
                 'Path to ROOT bin directory.',
                 NULL_PATH),

    ## gsl

    PathVariable('GSL_BINDIR',
                 'Path to gsl bin directory.',
                 NULL_PATH),

    ## HDF5

    PathVariable('HDF5_DIR',
                 'Path to HDF5 installation directory.',
                 NULL_PATH),

    ## nexus

    PathVariable('NEXUS_DIR',
                 'Path to nexus installation directory.',
                 NULL_PATH),


    ## The following vars shouldn't be defined by users unless they
    ## know what they are doing.

    ('CPPDEFINES',
     'Preprocessor definitions.',
     []),

    ('CCFLAGS',
     'General options passed to the compiler.',
     []),

    ('CPPFLAGS',
     'User-specified preprocessor options.',
     ['-g', '-fdiagnostics-color']),

    ('CXXFLAGS',
     'c++ compiler options.',
     ['-std=c++17']),

    ('CPPPATH',
     'List of directories where the include headers are located.',
     []),

    ('LIBPATH',
     'List of directories where the linked libraries are located.',
     []),

    ('LIBS',
     'List of libraries to link against.',
     []),

    ('LINKFLAGS',
     'User options passed to the linker.',
     [])

    )


## ###################################################################
## CONFIGURE BUILD

## Create a construction environment adding the build vars
env = Environment(variables=vars)

## Load the PATH variable of the user environment,
## to look for third-party library paths
env.PrependENVPath('PATH', os.environ['PATH'])

## Temporary: this line appears to be necessary in some Linux machines
## and will not be needed anymore with patch Geant4.11.0.1
env.PrependENVPath('LD_LIBRARY_PATH', os.environ['LD_LIBRARY_PATH'])

## If the LIBPATH buildvar (for instance) is not defined, the configure
## step has not been run yet
if not env['LIBPATH']:

    ## Create a Configure object that provides autoconf-like functionality
    conf = Configure(env, conf_dir='.sconf', log_file='.sconf/sconf.log')


    ## Geant4 configuration --------------------------------

    AssertG4Version(env['GEANT4_BINDIR'])

    if env['GEANT4_BINDIR'] != NULL_PATH:
        env.PrependENVPath('PATH', env['GEANT4_BINDIR'])

    env.ParseConfig('geant4-config --cflags --libs')

    if not conf.CheckCXXHeader('G4Event.hh'):
        Abort('Geant4 headers could not be found.')

    if not conf.CheckLib(library='G4global', language='CXX', autoadd=0):
        Abort('Geant4 libraries could not be found.')


    ## ROOT configuration ----------------------------------

    if env['ROOT_BINDIR'] != NULL_PATH:
        env.PrependENVPath('PATH', env['ROOT_BINDIR'])

    env.ParseConfig('root-config --cflags --libs')

    if not conf.CheckCXXHeader('TObject.h'):
        Abort('ROOT headers could not be found.')

    if not conf.CheckLib(library='Core', language='CXX', autoadd=0):
        Abort('ROOT libraries could not be found.')


    ## GSL configuration ---------------------------------

    if env['GSL_BINDIR'] != NULL_PATH:
        env.PrependENVPath('PATH', env['GSL_BINDIR'])

    env.ParseConfig('gsl-config --cflags --libs')

    if not conf.CheckCXXHeader('gsl/gsl_errno.h'):
        Abort('GSL headers not found.')

    if not conf.CheckLib(library='gsl', language='CXX', autoadd=0):
        Abort('GSL library not found.')


    ## HDF5 configuration ----------------------------------


    if env['HDF5_DIR'] == NULL_PATH:
        try:
            env['HDF5_DIR'] = os.environ['HDF5_DIR']
        except KeyError:
            Abort('HDF5 installation directory could not be found.')

    env.Append(LIBPATH = [env['HDF5_DIR']+'/lib'])
    env.Append(LIBS    = ['hdf5'])
    env.Append(CPPPATH = [env['HDF5_DIR']+'/include'])

    if not conf.CheckCXXHeader('hdf5.h'):
        Abort('HDF5 headers not found.')

    if not conf.CheckLib(library='hdf5', language='CXX', autoadd=0):
        Abort('HDF5 library not found.')


    ## NEXUS configuration -------------------------------
    env.ParseConfig('nexus-config --include --libdir --libs')

    if not conf.CheckCXXHeader('nexus/NexusApp.h'):
        Abort('NEXUS headers could not be found.')

    if not conf.CheckLib(library='nexus', language='CXX', autoadd=0):
        Abort('NEXUS libraries could not be found.')


    ## NEST configuration --------------------------
    env.Append( LIBPATH = [os.environ['NEST_LIB']] )
    env.Append( CPPPATH = [os.environ['NEST_INC']] )
    env.Append( CPPPATH = [os.environ['NEST_INC']+'/Detectors'] )
    env.Append( CPPPATH = [os.environ['NEST_INC']+'/NEST'] )
    env.Append( CPPPATH = [os.environ['NEST_INC']+'/NEST/G4'] )
    env.Append( CPPPATH = [os.environ['NEST_INC']+'/NEST/gcem_incl'] )
    env.Append(LIBS = ['NESTCore', 'NESTG4'])

    if not conf.CheckCXXHeader('NEST.hh'):
        Abort('NEST headers not found.')

    if not conf.CheckLib(library='NESTG4', language='CXX', autoadd=0):
        Abort('NEST libraries could not be found.')


## ##################################################################

    ## Force nexus to use C++17 standard
    if '-std=c++11' in env['CXXFLAGS']:
        env['CXXFLAGS'].remove('-std=c++11')
    if '-std=c++14' in env['CXXFLAGS']:
        env['CXXFLAGS'].remove('-std=c++14')


    env = conf.Finish()

vars.Save(BUILDVARS_FILE, env)


## ###################################################################
## BUILDING PETALOSIM

SRCDIR  = ['source/' + dir for dir in SRCDIR]

env.Append(CPPPATH = SRCDIR)

src = []
for d in SRCDIR:
    src += Glob(d+'/*.cc')

env['CXXCOMSTR']  = "Compiling $SOURCE"
env['LINKCOMSTR'] = "Linking $TARGET"

nexus = env.Program('bin/petalo', ['source/petalo.cc']+src)

TSTDIR = ['utils',
	  'example']
TSTDIR = ['source/tests/' + dir for dir in TSTDIR]

tst = []
for d in TSTDIR:
    tst += Glob(d+'/*.cc')

env.Append(CPPPATH = ['source/tests'])
nexus_test = env.Program('bin/petalo-test', ['source/petalo-test.cc']+tst+src)

Clean(nexus, 'buildvars.scons')
