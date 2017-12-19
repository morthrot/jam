# jam

*jam* allows you to distribute submitted commands across avaliable CPU.
As soon as all the commands are processed, jam is finished. Upon command completion the output is printed.
Built with [Qt](http://qt.io/).
Project [Homepage](http://tycho.sytes.net/projects/index.html#jam).

Direct usage: `jam <cmd> [arg] ... [arg_n] [ --- <cmd> [arg] ... [arg_n] ]`

Script usage:
```perl
#!/usr/bin/perl

$jam = "~/bin/jam";

$ffmpeg = `which ffmpeg`;
chomp $ffmpeg;

@filez = `ls | sort`;
@jams = ();

open(FILELIST,">files.txt") or die "Error creating files.txt\n";

foreach $file(@filez) {
  chomp $file;

  if($file =~ m/(([^\/]+)\.(mp3|wav))$/) {
    $old = $1;
    $new = $2 . ".m4b";

    push @jams, "$ffmpeg -i \"$old\" -map 0:0 -c:a aac -b:a 32k -f mp4 -y -ac 1 \"$new\"";
    print FILELIST "file '$new'\n";
    }
  }

close(FILELIST);

$enjam = $jam . " " . join(" --- ",@jams);
print "$enjam\n";
`$enjam`;

$enjoy = "$ffmpeg -f concat -i \"files.txt\" -acodec copy -f mp4 -y \"output.m4b\"";
print "$enjoy\n";
`$enjoy`;
```
