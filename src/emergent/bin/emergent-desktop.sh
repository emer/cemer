#!/bin/sh
cat <<EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=Emergent
GenericName=Neural network simulation system
Comment=Create and analyze complex, sophisticated models of the brain
TryExec=$1/bin/emergent
Exec=$1/bin/emergent &
Terminal=false
Categories=Application;Education;
Icon=$1/share/Emergent/images/emergent_32x32.png
EOF

