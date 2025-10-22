#!/usr/bin/env python3
import re

with open('src/Rendering/Renderer.cpp', 'r') as f:
    lines = f.readlines()

new_lines = []
for line in lines:
    # Fix vertex initialization patterns
    if '{{' in line and '}, {' in line and '}}' in line:
        # Convert {{a,b,c},{d,e,f},{g,h,i},{j,k}} to Vertex(Vector3(a,b,c), Vector3(d,e,f), Vector3(g,h,i), Vector3(j,k,0))
        # Extract the values
        match = re.match(r'\s*\{\{([^}]+)\}, \{([^}]+)\}, \{([^}]+)\}, \{([^}]+)\}\}(.*)', line)
        if match:
            pos = match.group(1)
            norm = match.group(2)
            col = match.group(3)
            tex = match.group(4)
            rest = match.group(5)
            # Convert tex coords if only 2 components
            tex_parts = tex.split(',')
            if len(tex_parts) == 2:
                tex = tex + ', 0'
            new_line = f'        Vertex(Vector3({pos}), Vector3({norm}), Vector3({col}), Vector3({tex})){rest}\n'
            new_lines.append(new_line)
        else:
            new_lines.append(line)
    else:
        new_lines.append(line)

with open('src/Rendering/Renderer.cpp', 'w') as f:
    f.writelines(new_lines)