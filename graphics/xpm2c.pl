#!/usr/bin/perl
#
# Quick hack to convert an xpm file to C code containing a palette and a pixel
# array directly usable by an 8 bit framebuffer.
#

use strict;

my $input_file = $ARGV[0];

open my $fh, '<', $input_file || die "can't open $input_file";

my $varname = $ARGV[1];

my ($width, $height, $palette_size, $chars_per_pixel);
my @palette;
my %char_to_index;
my @pixels;

# First loop: find header line defining image dimensionse
while(<$fh>)
{
    if (/^"(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\",?$/)
    {
        $width = $1;
        $height = $2;
        $palette_size = $3;
        $chars_per_pixel = $4;
        last;
    }
}

# Second loop: read palette:
while (<$fh>)
{
    if(/^"(.{$chars_per_pixel})\s+c\s+#([0-9a-f]{2})([0-9a-f]{2})([0-9a-f]{2})",?$/)
    {
        $char_to_index{$1} = scalar(@palette);
        push @palette, "0x00$4$3$2"; # Palette is RGB, but the Raspberry PI requires ABGR
        last if scalar(@palette) == $palette_size;
    }
    else
    {
        warn "Pal Ignoring unreckognized line $.: $_";
    }
}

#Third loop read pixels
while (<$fh>)
{
    if (/^"([^"]+)",?$/)
    {
        # take everey $chars_per_pixel characters and look them up in %char_to_index and append each index to the @pixels array
        push @pixels, map { $char_to_index{$_} } ($1 =~ m/.{$chars_per_pixel}/g);
        last if scalar(@pixels) == $width * $height;
    }
    else
    {
        warn "PX Ignoring unreckognized line $.: $_";
    }
}

print "#include <circle/types.h>\n\n";
print "unsigned ${varname}_width = $width;\n";
print "unsigned ${varname}_height = $height;\n\n";
print "u32 ${varname}_palette[] = {";

for (my $i=0; $i < @palette; $i++)
{
    print "\n    " if $i % 8 == 0;
    print $palette[$i];
    print ", " unless $i == $#palette;
}
print "\n};\n\n";

print "u8 ${varname}_pixels[] = {";
for (my $i=0; $i < @pixels; $i++)
{
    print "\n    " if $i % 16 == 0;
    printf "0x%02x", $pixels[$i];
    print ", " unless $i == $#pixels;
}
print "\n};\n\n";
