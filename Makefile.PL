use 5.010001;
use ExtUtils::MakeMaker;

my $extdir = 'github-markdown';
my $myextlib = File::Spec->catfile($extdir, 'libgithubmarkdown.a');

sub MY::postamble {
    return << "EOM";
\$(MYEXTLIB):
	(cd $extdir; make)
EOM
}
#gh-markdown.c

WriteMakefile(
    NAME              => 'Text::Markdown::Github',
    VERSION_FROM      => 'lib/Text/Markdown/Github.pm', # finds $VERSION
    PREREQ_PM         => {}, # e.g., Module::Name => 1.1
    ($] >= 5.005 ?     ## Add these new keywords supported since 5.005
      (ABSTRACT_FROM  => 'lib/Text/Markdown/Github.pm', # retrieve abstract from module
       AUTHOR         => 'woremacx <woremacx@gmail.com>') : ()),
    LIBS              => '-L' . $extdir,
    INC               => '-I. -I' . $extdir,
    MYEXTLIB          => $myextlib,
    clean             => { FILES => $clean_files },
    DEFINE            => '', # e.g., '-DHAVE_SOMETHING'
);
