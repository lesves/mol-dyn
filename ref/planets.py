from astropy.time import Time
from astropy.coordinates import solar_system_ephemeris, get_body_barycentric_posvel
from astropy import units as u
from astropy import constants as const
import numpy as np


t = Time("2024-11-05 23:42")

names = [
	"sun",
	"mercury",
	"venus",
	"earth",
	"mars",
	"jupiter",
	"saturn",
	"uranus",
	"neptune"
]
positions = []
velocities = []

time_unit = u.day
dist_unit = u.AU
mass_unit = u.earthMass

G = np.array(const.G.to(dist_unit**3/mass_unit/time_unit**2))

with solar_system_ephemeris.set("builtin"):
	for name in names:
		pos, vel = get_body_barycentric_posvel(name, t)
		positions.append(pos.xyz.to(dist_unit))
		velocities.append(vel.xyz.to(dist_unit/time_unit))

positions = np.array(positions)
velocities = np.array(velocities)

masses = np.array([
	333060,
	0.0553, 
	0.815, 
	1, 
	0.1075,
	317.8,
	95.2,
	14.6,
	17.2,
])
