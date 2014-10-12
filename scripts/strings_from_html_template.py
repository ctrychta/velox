def chunks(s):
    CHUNK_SIZE = 10000
    return (s[i:i+CHUNK_SIZE] for i in range(0, len(s), CHUNK_SIZE))

def write_strings(dest, tpl):
    sep = ''
    for c in chunks(tpl):
        dest.write(sep + 'R"***^***(' + c + ')***^***"')
        sep = ',\n'
    dest.write('\n')

def main():
    PLACEHOLDER = '/*****PLACEHOLDER FOR benchmarkData*****/'
    with open('html_report_template.html') as f, \
        open('html_template_begin.tpl', 'w') as tpl_begin, \
        open('html_template_end.tpl', 'w') as tpl_end:
        template = f.read();
        template_begin, template_end = template.split(PLACEHOLDER)
        write_strings(tpl_begin, template_begin);
        write_strings(tpl_end, template_end)

if __name__ == "__main__":
    main()
