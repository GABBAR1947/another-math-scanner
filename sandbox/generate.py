import string
from random import randint


symbols = [
'\\alpha', '\\beta', '\\gamma', '\\Gamma', '\\delta', '\\Delta', '\\epsilon', '\\zeta', '\\eta', '\\theta', '\\Theta', '\\iota', '\\kappa', '\\lambda', '\\Lambda', '\\mu', '\\nu', '\\omicron', '\\pi', '\\Pi', '\\rho', '\\sigma', '\\Sigma', '\\tau', '\\upsilon', '\\Upsilon', '\\phi', '\\Phi', '\\chi', '\\psi', '\\Psi', '\\omega', '\\Omega',
]

range_ops = ['\\sum', '\\prod', '\\int']


printable = list(string.printable)
universe = printable + symbols + range_ops





def get_links(c):
    fonts = ['cm', 'cs', 'jvn', 'phv']
    fcodes = [''] + list(map(lambda x: '\\fn_'+x, fonts))
    link = 'https://latex.codecogs.com/svg.download?'
    gen_links = lambda x: (' '.join([link+x,'\\huge', c]), c, x)
    links = list(map(gen_links, fcodes))
    return links
    



commands = []

def generate_command(x):
    link, char, font = x
    font = font.replace("\\fn_", "")
    char = char.replace("\\", "")
    filename = char + font + '.svg'
    dl = 'wget "%s" -O %s'%(link,  filename)
    sleep = 'sleep %ds'%(randint(1, 5))
    command = "%s && %s"%(sleep, dl)
    return command

for s in universe:
    commands = list(map(generate_command, get_links(s)))
    for cmd in commands:
        print(cmd)




