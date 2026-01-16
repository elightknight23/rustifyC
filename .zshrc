export PATH="$PATH:$HOME/development/flutter/bin"
export JAVA_HOME=$(/usr/libexec/java_home -v 22.0.1)
export PATH=$JAVA_HOME/bin:$PATH
export PATH=$PATH:/Users/nithikdeva/.spicetify
# LLVM 15
export PATH="/opt/homebrew/opt/llvm@15/bin:$PATH"
export LDFLAGS="-L/opt/homebrew/opt/llvm@15/lib"
export CPPFLAGS="-I/opt/homebrew/opt/llvm@15/include"


export NVM_DIR="$HOME/.nvm"
[ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh"  # This loads nvm
[ -s "$NVM_DIR/bash_completion" ] && \. "$NVM_DIR/bash_completion"  # This loads nvm bash_completion
export PATH="$PATH":"$HOME/.pub-cache/bin"
