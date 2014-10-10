#!/bin/bash

# BLOCK COMMENT
: <<'END'
END

easyfi () {
	./move_files.sh
	cd $1
	./easy.py $2 $2.t >> ../../$2Output.txt
	echo "" >> ../../$2Output.txt
	cd -
}

radFileName="star"
hjpdFileName="joint_displace"
hodFileName="trajectories"
dtrain="dataset/train/"
dtest="dataset/test/"
tools="libsvm-3.18/tools"


if [[ ! -x "$radFileName" ]]
	then
	echo "File ${radFileName} does not exist"
	exit 1
fi
if [[ ! -x "$hjpdFileName" ]]
	then
	echo "File ${hjpdFileName} does not exist"
	exit 1
fi
if [[ ! -x "$hodFileName" ]]
	then
	echo "File ${hodFileName} does not exist"
	exit 1
fi

echo "" > radOutput.txt
echo "" > hjpdOutput.txt
echo "" > hodOutput.txt

#################################################
###				Rad File Tests                ###
#################################################

# Default
echo "Rad with default values" >> radOutput.txt
./$radFileName $dtrain $dtest
easyfi "$tools" "rad"

# Wrists & Ankles
echo "Rad with wrists and ankles" >> radOutput.txt
./$radFileName -la 10 -ll 18 -ra 6 -rl 14 $dtrain $dtest
easyfi "$tools" "rad"

# Wrists & Feet
echo "Rad with wrists and feet" >> radOutput.txt
./$radFileName -la 10 -ra 6 $dtrain $dtest
easyfi "$tools" "rad"

# Hands & Ankles
echo "Rad with hands and ankles" >> radOutput.txt
./$radFileName -rl 14 -ll 18 $dtrain $dtest
easyfi "$tools" "rad"

# Chaning Distance lower bound
for i in `seq 0.00 0.05 1.1`;
do
	echo "Rad with distance lower changing only, current: ${i}" >> radOutput.txt
	./$radFileName -dl $i $dtrain $dtest
	easyfi "$tools" "rad"
done

# Changing Distance upper bound
for i in `seq 2.2 -0.05 1.1`;
do
	echo "Rad with distance upper changing only, current: ${i}" >> radOutput.txt
	./$radFileName -du $i $dtrain $dtest
	easyfi "$tools" "rad"
done

# Changing Bins
for bins in `seq 5 5 50`;
do
	echo "Rad with changing bins (${bins})" >> radOutput.txt
	./$radFileName -db $bins $dtrain $dtest
	easyfi "$tools" "rad" 
done

# Changing lower and upper bound, same rate of change
for i in `seq 0.00 0.05 0.55`;
do
	lower=$(bc -l <<< "0.00+$i")
	upper=$(bc -l <<< "2.20-$i")
	echo "Lower (${lower}) and upper (${upper})"
	# echo "Rad with distance lower (${lower}) and upper (${upper}) changing constant 0.05" >> radOutput.txt
	# ./$radFileName -dl $lower -du $upper $dtrain $dtest
	# easyfi "$tools" "rad" 
done

# Changing lower and upper bound, same rate of change
# Bin changing at each lower/upper change
for i in `seq 0.00 0.05 0.55`;
do
	lower=$(bc -l <<< "0.00+$i")
	upper=$(bc -l <<< "2.20-$i")
	for bins in `seq 5 5 50`;
	do
		echo "Rad with distance lower (${lower}) and upper (${upper}) changing constant 0.05 and bins (${bins})" >> radOutput.txt
		./$radFileName -dl $lower -du $upper -db $bins $dtrain $dtest
		easyfi "$tools" "rad" 
	done
done

# Lower bound with Wrists & Ankles
for i in `seq 0.00 0.05 1.1`;
do
	echo "Rad with distance lower changing only, current: ${i}. wrists/ankles" >> radOutput.txt
	./$radFileName -dl $i -la 10 -ll 18 -ra 6 -rl 14 $dtrain $dtest
	easyfi "$tools" "rad"
done

# Upper bound with Wrists & Ankles
for i in `seq 2.2 -0.05 1.1`;
do
	echo "Rad with distance upper changing only, current: ${i}. wrists/ankles" >> radOutput.txt
	./$radFileName -du $i -la 10 -ll 18 -ra 6 -rl 14 $dtrain $dtest
	easyfi "$tools" "rad"
done

# Bins with Wrists & Ankles
for bins in `seq 5 5 50`;
do
	echo "Rad with changing bins (${bins}). wrists/ankles" >> radOutput.txt
	./$radFileName -db $bins -la 10 -ll 18 -ra 6 -rl 14 $dtrain $dtest
	easyfi "$tools" "rad" 
done

# Lower/Upper changes with Wrists & Ankles
for i in `seq 0.00 0.05 0.55`;
do
	lower=$(bc -l <<< "0.00+$i")
	upper=$(bc -l <<< "2.20-$i")
	echo "Rad with distance lower (${lower}) and upper (${upper}) changing constant 0.05. wrists/ankles" >> radOutput.txt
	./$radFileName -dl $lower -du $upper -la 10 -ll 18 -ra 6 -rl 14 $dtrain $dtest
	easyfi "$tools" "rad" 
done

# Lower/Upper/Bin changes with Wrists & Ankles
for i in `seq 0.00 0.05 0.55`;
do
	lower=$(bc -l <<< "0.00+$i")
	upper=$(bc -l <<< "2.20-$i")
	for bins in `seq 5 5 50`;
	do
		echo "Rad with distance lower (${lower}) and upper (${upper}) changing constant 0.05 and bins (${bins}). wrists/ankles" >> radOutput.txt
		./$radFileName -dl $lower -du $upper -db $bins -la 10 -ll 18 -ra 6 -rl 14 $dtrain $dtest
		easyfi "$tools" "rad" 
	done
done

####

# Chaning angle lower bound
for i in `seq 0.00 0.05 1.6`;
do
	echo "Rad with angle lower changing only, current: ${i}" >> radOutput.txt
	./$radFileName -al $i $dtrain $dtest
	easyfi "$tools" "rad"
done

# Changing angle upper bound
for i in `seq 3.2 -0.05 1.6`;
do
	echo "Rad with angle upper changing only, current: ${i}" >> radOutput.txt
	./$radFileName -au $i $dtrain $dtest
	easyfi "$tools" "rad"
done

# Changing Bins
for bins in `seq 5 5 50`;
do
	echo "Rad with changing bins (${bins})" >> radOutput.txt
	./$radFileName -ab $bins $dtrain $dtest
	easyfi "$tools" "rad" 
done

# Changing lower and upper bound, same rate of change
for i in `seq 0.00 0.05 0.8`;
do
	lower=$(bc -l <<< "0.00+$i")
	upper=$(bc -l <<< "3.20-$i")
	echo "Rad with angle lower (${lower}) and upper (${upper}) changing constant 0.05" >> radOutput.txt
	./$radFileName -al $lower -au $upper $dtrain $dtest
	easyfi "$tools" "rad" 
done

# Changing lower and upper bound, same rate of change
# Bin changing at each lower/upper change
for i in `seq 0.00 0.05 0.8`;
do
	lower=$(bc -l <<< "0.00+$i")
	upper=$(bc -l <<< "3.20-$i")
	for bins in `seq 5 5 50`;
	do
		echo "Rad with angle lower (${lower}) and upper (${upper}) changing constant 0.05 and bins (${bins})" >> radOutput.txt
		./$radFileName -al $lower -au $upper -ab $bins $dtrain $dtest
		easyfi "$tools" "rad" 
	done
done

# Lower bound with Wrists & Ankles
for i in `seq 0.00 0.05 1.6`;
do
	echo "Rad with angle lower changing only, current: ${i}" >> radOutput.txt
	./$radFileName -al $i -la 10 -ll 18 -ra 6 -rl 14 $dtrain $dtest
	easyfi "$tools" "rad"
done

# Upper bound with Wrists & Ankles
for i in `seq 3.2 -0.05 1.6`;
do
	echo "Rad with angle upper changing only, current: ${i}" >> radOutput.txt
	./$radFileName -au $i -la 10 -ll 18 -ra 6 -rl 14 $dtrain $dtest
	easyfi "$tools" "rad"
done

# Bins with Wrists & Ankles
for bins in `seq 5 5 50`;
do
	echo "Rad with changing bins (${bins})" >> radOutput.txt
	./$radFileName -ab $bins -la 10 -ll 18 -ra 6 -rl 14 $dtrain $dtest
	easyfi "$tools" "rad" 
done

# Lower/Upper changes with Wrists & Ankles
for i in `seq 0.00 0.05 0.8`;
do
	lower=$(bc -l <<< "0.00+$i")
	upper=$(bc -l <<< "3.20-$i")
	echo "Rad with angle lower (${lower}) and upper (${upper}) changing constant 0.05" >> radOutput.txt
	./$radFileName -al $lower -au $upper -la 10 -ll 18 -ra 6 -rl 14 $dtrain $dtest
	easyfi "$tools" "rad" 
done

# Lower/Upper/Bin changes with Wrists & Ankles
for i in `seq 0.00 0.05 0.8`;
do
	lower=$(bc -l <<< "0.00+$i")
	upper=$(bc -l <<< "3.20-$i")
	for bins in `seq 5 5 50`;
	do
		echo "Rad with angle lower (${lower}) and upper (${upper}) changing constant 0.05 and bins (${bins})" >> radOutput.txt
		./$radFileName -al $lower -au $upper -ab $bins -la 10 -ll 18 -ra 6 -rl 14 $dtrain $dtest
		easyfi "$tools" "rad" 
	done
done

#################################################
###			Rad File Test Complete            ###
#################################################

#################################################
###				Hjpd File Tests   		      ###
#################################################

# Default
echo "Hjpd with default values" >> hjpdOutput.txt
./$hjpdFileName $dtrain $dtest
easyfi "$tools" "hjpd"

# Chaning angle lower bound
for i in `seq -1.10 0.05 0.1`;
do
	echo "Hjpd with angle lower changing only, current: ${i}" >> hjpdOutput.txt
	./$hjpdFileName -l $i $dtrain $dtest
	easyfi "$tools" "hjpd"
done

# Changing angle upper bound
for i in `seq 1.3 -0.05 0.1`;
do
	echo "Hjpd with angle upper changing only, current: ${i}" >> hjpdOutput.txt
	./$hjpdFileName -u $i $dtrain $dtest
	easyfi "$tools" "hjpd"
done

# Changing Bins
for bins in `seq 5 5 50`;
do
	echo "Hjpd with changing bins (${bins})" >> hjpdOutput.txt
	./$hjpdFileName -b $bins $dtrain $dtest
	easyfi "$tools" "hjpd" 
done

# Changing lower and upper bound, same rate of change
for i in `seq 0.00 0.05 0.6`;
do
	lower=$(bc -l <<< "-1.10+$i")
	upper=$(bc -l <<< "1.30-$i")
	echo "Hjpd with angle lower (${lower}) and upper (${upper}) changing constant 0.05" >> hjpdOutput.txt
	./$hjpdFileName -l $lower -u $upper $dtrain $dtest
	easyfi "$tools" "hjpd" 
done

# Changing lower and upper bound, same rate of change
# Bin changing at each lower/upper change
for i in `seq 0.00 0.05 0.6`;
do
	lower=$(bc -l <<< "-1.10+$i")
	upper=$(bc -l <<< "1.30-$i")
	for bins in `seq 5 5 50`;
	do
		echo "Hjpd with angle lower (${lower}) and upper (${upper}) changing constant 0.05 and bins (${bins})" >> hjpdOutput.txt
		./$hjpdFileName -l $lower -u $upper -b $bins $dtrain $dtest
		easyfi "$tools" "hjpd" 
	done
done

# Change reference Joint
for refJoint in `seq 0 20`;
do
	echo "Hjpd with different reference joint (${refJoint})." >> hjpdOutput.txt
	./$hjpdFileName -r refJoint $dtrain $dtest
	easyfi "$tools" "hjpd"
done

# Changing lower and upper bound, same rate of change
# Bin changing at each lower/upper change
# Reference Joint changing
for i in `seq 0.00 0.05 0.6`;
do
	lower=$(bc -l <<< "-1.10+$i")
	upper=$(bc -l <<< "1.30-$i")
	for bins in `seq 5 5 50`;
	do
		for refJoint in `seq 0 20`;
		do
			echo "Hjpd with angle lower (${lower}) and upper (${upper}) changing constant 0.05, bins (${bins}) and refJoint (${refJoint})" >> hjpdOutput.txt
			./$hjpdFileName -l $lower -u $upper -b $bins -r $refJoint $dtrain $dtest
			easyfi "$tools" "hjpd" 
		done
	done
done

#################################################
###			Hjpd File Tests Complete          ###
#################################################


#################################################
###				Hod File Tests 	  	          ###
#################################################

# Default
echo "Hod with default values" >> hodOutput.txt
./$hodFileName $dtrain $dtest
easyfi "$tools" "hod"

# Changing Bins
for bins in `seq 4 2 16`;
do
	echo "Hod with changing bins (${bins})" >> hodOutput.txt
	./$hodFileName -b $bins $dtrain $dtest
	easyfi "$tools" "hod" 
done

#################################################
###			Hod File Tests Complete	  	      ###
#################################################
