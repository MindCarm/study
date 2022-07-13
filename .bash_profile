# .bash_profile

# Get the aliases and functions
if [ -f ~/.bashrc ]; then
	. ~/.bashrc
fi

# User specific environment and startup programs

alias so='source ~/.bash_profile'
alias psg='ps -ef | grep -v grep | grep '