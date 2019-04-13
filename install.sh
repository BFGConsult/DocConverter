system=$(uname)

case $system
in
    Linux)
	distributor=$(lsb_release -i| cut -d: -f2)
	# remove leading whitespace characters
	distributor="${distributor#"${distributor%%[![:space:]]*}"}"
	# remove trailing whitespace characters
	distributor="${distributor%"${distributor##*[![:space:]]}"}"   
	case $distributor
	in
	    Ubuntu)
		sudo apt install jq curl
		;;
	    *)
		echo "'$distributor' not supported"
		;;
	esac
	;;
    *)
	echo "'$system' not supported"
esac
