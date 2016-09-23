_cwdof()
{
	local cur prev opts
	COMPREPLY=()
	cur="${COMP_WORDS[COMP_CWORD]}"					# current word
	prev="${COMP_WORDS[COMP_CWORD-1]}" 				# previous word
	opts="`ps -o cmd -u $USER | grep ^/ -v  | cut -d ' ' -f 1  `"	# all process names owned by user

	if [[ ${cur} == * ]] ; then
		COMPREPLY=( $(compgen -W "${opts}" -- ${cur}))
		return 0
	fi
}

complete -W "-n -p" -F _cwdof cwdof
