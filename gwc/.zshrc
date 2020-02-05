# Personal configuration for cross-building in a chroot.

# To work with lmi, it is crucial to source 'set_toolchain.sh'. It's
# deliberately not sourced here, so that it can assume that some
# things have already been set up: for instance, it assumes that
#   /usr/share/libtool/build-aux/config.guess
# is present, which need not be the case in a brand-new chroot.

# Minimal system path.
export MINIMAL_PATH="/usr/bin:/bin:/usr/sbin:/sbin"
export PATH="$MINIMAL_PATH"

# At a regular user prompt, outside the chroot, do this:
#   $ echo $DISPLAY
# and replace :0.0 below with the string it returns:
export DISPLAY=":0.0"

# Make the X clipboard available to root, for vim only. See:
#   https://lists.nongnu.org/archive/html/lmi/2019-10/msg00000.html
if [ "$(id -u)" -eq 0 ]; then
  alias vim='XAUTHORITY=/home/greg/.Xauthority vim'
fi

# Something like
#  "--jobs=$(nproc || sysctl -n hw.ncpu || getconf _NPROCESSORS_ONLN)"
# could be used instead for other *nix systems:
export coefficiency="--jobs=$(nproc)"

# This should be unnecessary:
# export TZ=UCT

# redhat-based distributions may lack 'C.UTF-8'--see:
#   https://bugzilla.redhat.com/show_bug.cgi?id=902094
if locale -a | grep --quiet C.UTF-8; then
  pref_lc=C.UTF-8; else
  pref_lc=en_US.UTF-8;
fi
export LANG=en_US.UTF-8 LC_ALL=$pref_lc LC_TIME=en_DK.UTF-8 LC_COLLATE=$pref_lc
# Instead of assigning those variables, this is generally preferable:
#   update-locale LANG=en_US.UTF-8 LC_TIME=en_DK.UTF-8 LC_COLLATE=C.UTF-8
# but neither the chroot's nor the host's /etc/default/locale is
# sourced by schroot, which strives to set as few environment
# variables as possible.

# This ought to be unnecessary because $LANG contains the string
# "UTF-8"--see:
#   https://lists.nongnu.org/archive/html/lmi/2018-03/msg00049.html
# but it seems to be necessary nevertheless.
export LESSCHARSET=utf-8

# Use vim keybindings.
bindkey -v

# This seems to be set by default:
# bindkey '\e[3~' delete-char      # Del

# Replace the default vim keybinding, to reduce astonishment.
bindkey '^?' backward-delete-char # Backspace

# Explicitly bind these--see:
#   https://lists.nongnu.org/archive/html/lmi/2019-10/msg00032.html
bindkey '\e[H' beginning-of-line # Home
bindkey '\e[F' end-of-line       # End
bindkey '\e[1;5D' backward-word  # Ctrl-left
bindkey '\e[1;5C' forward-word   # Ctrl-right
bindkey '\e[1;3D' backward-word  # Alt-left
bindkey '\e[1;3C' forward-word   # Alt-right

# Bind those in the 'vicmd' keymap, too:
bindkey -M vicmd '\e[H' beginning-of-line # Home
bindkey -M vicmd '\e[F' end-of-line       # End
bindkey -M vicmd '\e[1;5D' backward-word  # Ctrl-left
bindkey -M vicmd '\e[1;5C' forward-word   # Ctrl-right
bindkey -M vicmd '\e[1;3D' backward-word  # Alt-left
bindkey -M vicmd '\e[1;3C' forward-word   # Alt-right

# Enable useful features that emacs mode binds by default.
# Binding '^R' here doesn't interfere with '^R' (undo) in vicmd mode.
bindkey '^R' history-incremental-search-backward # emacs Ctrl-R
bindkey '^F' history-incremental-search-forward  # emacs Ctrl-S
bindkey '^G' send-break                          # emacs Ctrl-G
bindkey '\eq' push-line                          # emacs Esc-Q

function zle-line-init zle-keymap-select {
    local local_prompt='%d[%?]%(!.#.$)'
    if [[ ${KEYMAP} == vicmd ]]; then
        prompt="%F{yellow}${local_prompt}%f"
    else
        prompt="${local_prompt}"
    fi
    zle reset-prompt
}

zle -N zle-line-init
zle -N zle-keymap-select

HISTSIZE=1000
SAVEHIST=1000
HISTFILE=~/.history
setopt HIST_IGNORE_DUPS
setopt HIST_IGNORE_SPACE

autoload -U compinit
compinit -u

# This alias is explained in 'tabs/konsole_tabs'.
alias asdf='. ./startup_script'
