def gen_convert_command(x):
    fcodes = ["phv", "jvn", "cs", "cm", ""]
    svg = '.svg'
    for code in fcodes:
        ccode = code + svg
        if ( len(ccode) < len(x)):
            diff = len(x) - len(ccode)
            ext = x[diff:]
            if (ccode == ext):
                fromName = x
                toName = x[:diff];
                if code: toName += '_'+ code
                toName += '.png'
                #inkscape -z -e test.png -w 1024 -h 1024 test.svg
                #cmd = "convert %s %s"%(fromName, toName)
                cmd = 'inkscape -z -e "pngs/%s" -w 64 -h 64 "%s" -y 1.0 -b #ffffff'%(toName, fromName)
                return cmd
    return 'echo "Do nothing"'



fnames = open("files.list", "r")
names = list(map(lambda x: x.strip(), fnames))

cmds = list(map(gen_convert_command, names))
for cmd in cmds:
    print(cmd)


