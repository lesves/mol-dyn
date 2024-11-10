import sys
sys.path.insert(0, "../ref")
import planets

print(f"time_unit={planets.time_unit} dist_unit={planets.dist_unit} mass_unit={planets.mass_unit} G={planets.G}")
print("name,pos,vel,mass")
for name, pos, vel, mass in zip(planets.names, planets.positions, planets.velocities, planets.masses):
	print(
		name, 
		pos[0], pos[1], pos[2], 
		vel[0], vel[1], vel[2], 
		mass,
		sep=","
	)
