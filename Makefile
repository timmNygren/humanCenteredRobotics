CXX = g++
CFLAGS = -Wall -g

all: star joint_displace trajectories

star: rad.cpp point3d.cpp bin.cpp
	${CXX} ${CFLAGS} rad.cpp point3d.cpp bin.cpp -o star

joint_displace: hjpd.cpp point3d.cpp bin.cpp
	${CXX} ${CFLAGS} hjpd.cpp point3d.cpp bin.cpp -o joint_displace

trajectories: hod.cpp point2d.cpp bin.cpp
	${CXX} ${CFLAGS} hod.cpp point2d.cpp point3d.cpp bin.cpp -o trajectories

clean:
	$(RM) star
	$(RM) joint_displace
	$(RM) trajectories
	rm -rf star.dSYM/
	rm -rf joint_displace.dSYM/
	rm -rf trajectories.dSYM/

fileClean:
	$(RM) rad
	$(RM) rad.t
	$(RM) hjpd
	$(RM) hjpd.t
	$(RM) hod
	$(RM) hod.t
	$(RM) *Output.txt