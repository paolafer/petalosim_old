import pytest

import os
import subprocess


@pytest.mark.order(1)
def test_create_nexus_output_file_full_body(config_tmpdir, output_tmpdir, PETALODIR, base_name_full_body):

     init_text = f"""
/PhysicsList/RegisterPhysics G4EmStandardPhysics_option4
/PhysicsList/RegisterPhysics G4DecayPhysics
/PhysicsList/RegisterPhysics G4RadioactiveDecayPhysics
/PhysicsList/RegisterPhysics G4OpticalPhysics
/PhysicsList/RegisterPhysics PetaloPhysics
/PhysicsList/RegisterPhysics G4StepLimiterPhysics

### GEOMETRY
/nexus/RegisterGeometry FullRingInfinity

### GENERATOR
/nexus/RegisterGenerator Back2backGammas

### ACTIONS
/nexus/RegisterRunAction DefaultRunAction
/nexus/RegisterEventAction PetaloEventAction
/nexus/RegisterTrackingAction PetaloTrackingAction

/nexus/RegisterPersistencyManager PetaloPersistencyManager

/nexus/RegisterMacro {config_tmpdir}/{base_name_full_body}.config.mac
"""
     init_path = os.path.join(config_tmpdir, base_name_full_body+'.init.mac')
     init_file = open(init_path,'w')
     init_file.write(init_text)
     init_file.close()

     config_text = f"""
/run/verbose 1
/event/verbose 0
/tracking/verbose 0

/process/em/verbose 0

/Geometry/FullRingInfinity/depth 3. cm
/Geometry/FullRingInfinity/sipm_pitch 7. mm
/Geometry/FullRingInfinity/inner_radius 380. mm
/Geometry/FullRingInfinity/sipm_rows 278
/Geometry/FullRingInfinity/instrumented_faces 1
/Geometry/FullRingInfinity/specific_vertex 0. 0. 0. cm

/Geometry/SiPMpet/efficiency 0.2
/Geometry/SiPMpet/visibility true
/Geometry/SiPMpet/time_binning 5. picosecond
/Geometry/SiPMpet/size 6. mm

/Generator/Back2back/region AD_HOC

/process/optical/processActivation Cerenkov false

/nexus/persistency/outputFile {output_tmpdir}/{base_name_full_body}
/nexus/random_seed 16062020

"""

     config_path = os.path.join(config_tmpdir, base_name_full_body+'.config.mac')
     config_file = open(config_path,'w')
     config_file.write(config_text)
     config_file.close()

     my_env    = os.environ
     petalo_exe = PETALODIR + '/bin/petalo'
     command   = [petalo_exe, '-b', '-n', '20', init_path]
     p         = subprocess.run(command, check=True, env=my_env)


@pytest.mark.order(2)
def test_create_petalo_output_file_ring_tiles(config_tmpdir, output_tmpdir, PETALODIR, base_name_ring_tiles):

     init_text = f"""
/PhysicsList/RegisterPhysics G4EmStandardPhysics_option4
/PhysicsList/RegisterPhysics G4DecayPhysics
/PhysicsList/RegisterPhysics G4RadioactiveDecayPhysics
/PhysicsList/RegisterPhysics G4OpticalPhysics
/PhysicsList/RegisterPhysics PetaloPhysics
/PhysicsList/RegisterPhysics G4StepLimiterPhysics

### GEOMETRY
/nexus/RegisterGeometry FullRingTiles

### GENERATOR
/nexus/RegisterGenerator Back2backGammas

### ACTIONS
/nexus/RegisterRunAction DefaultRunAction
/nexus/RegisterEventAction PetaloEventAction
/nexus/RegisterTrackingAction PetaloTrackingAction

/nexus/RegisterPersistencyManager PetaloPersistencyManager

/nexus/RegisterMacro {config_tmpdir}/{base_name_ring_tiles}.config.mac
"""
     init_path = os.path.join(config_tmpdir, base_name_ring_tiles+'.init.mac')
     init_file = open(init_path,'w')
     init_file.write(init_text)
     init_file.close()

     config_text = f"""
/run/verbose 1
/event/verbose 0
/tracking/verbose 0

/process/em/verbose 0

/Geometry/FullRingTiles/depth 3. cm
/Geometry/FullRingTiles/inner_radius 165. mm
/Geometry/FullRingTiles/tile_rows 2
/Geometry/FullRingTiles/instrumented_faces 1

/Geometry/SiPMpet/efficiency 0.2
/Geometry/SiPMpet/visibility true
/Geometry/SiPMpet/time_binning 5. picosecond
/Geometry/SiPMpet/size 3. mm

/Generator/Back2back/region CENTER

/process/optical/processActivation Cerenkov false

/nexus/persistency/outputFile {output_tmpdir}/{base_name_ring_tiles}
/nexus/random_seed 16062020

"""

     config_path = os.path.join(config_tmpdir, base_name_ring_tiles+'.config.mac')
     config_file = open(config_path,'w')
     config_file.write(config_text)
     config_file.close()

     my_env    = os.environ
     petalo_exe = PETALODIR + '/bin/petalo'
     command   = [petalo_exe, '-b', '-n', '20', init_path]
     p         = subprocess.run(command, check=True, env=my_env)


@pytest.mark.order(3)
def test_create_petalo_output_file_pet_box_all_tiles(config_tmpdir, output_tmpdir, PETALODIR, petalosim_pet_box_params):

     _, base_name, tile_type1, tile_type2, _, _, _, _, _, min_charge_evt = petalosim_pet_box_params

     init_text = f"""
/PhysicsList/RegisterPhysics G4EmStandardPhysics_option4
/PhysicsList/RegisterPhysics G4DecayPhysics
/PhysicsList/RegisterPhysics G4RadioactiveDecayPhysics
/PhysicsList/RegisterPhysics G4OpticalPhysics
/PhysicsList/RegisterPhysics PetaloPhysics
/PhysicsList/RegisterPhysics G4StepLimiterPhysics

### GEOMETRY
/nexus/RegisterGeometry PetBox

### GENERATOR
/nexus/RegisterGenerator IonGenerator
#/nexus/RegisterGenerator SingleParticleGenerator

### ACTIONS
/nexus/RegisterRunAction DefaultRunAction
/nexus/RegisterEventAction PetSensorsEventAction
/nexus/RegisterTrackingAction PetaloTrackingAction

/nexus/RegisterPersistencyManager PetaloPersistencyManager

/nexus/RegisterMacro {config_tmpdir}/{base_name}.config.mac
"""
     init_path = os.path.join(config_tmpdir, base_name+'.init.mac')
     init_file = open(init_path,'w')
     init_file.write(init_text)
     init_file.close()

     config_text = f"""
/run/verbose 1
/event/verbose 0
/tracking/verbose 0

/process/em/verbose 0

/Geometry/PetBox/tile_type_d {tile_type1}
/Geometry/PetBox/tile_type_c {tile_type2}
/Geometry/PetBox/single_tile_coinc_plane 0
/Geometry/PetBox/tile_refl 0.
/Geometry/PetBox/sipm_time_binning 5. picosecond
/Geometry/PetBox/sipm_pde 0.5

/Generator/IonGenerator/region SOURCE
/Generator/IonGenerator/atomic_number 11
/Generator/IonGenerator/mass_number 22

/Actions/PetSensorsEventAction/min_charge {min_charge_evt}

/process/optical/processActivation Cerenkov false

/nexus/persistency/outputFile {output_tmpdir}/{base_name}

/nexus/random_seed 23102020
"""

     config_path = os.path.join(config_tmpdir, base_name+'.config.mac')
     config_file = open(config_path,'w')
     config_file.write(config_text)
     config_file.close()

     my_env    = os.environ
     petalo_exe = PETALODIR + '/bin/petalo'
     command   = [petalo_exe, '-b', '-n', '20', init_path]
     p         = subprocess.run(command, check=True, env=my_env)
