# Copyright (c) 2001, Stanford University
# All rights reserved.
#
# See the file LICENSE.txt for information on redistributing this software

sub gendiffcode;
$name = shift @ARGV;

print << 'EOF';
/* This code is AUTOGENERATED!!! */

#include "cr_glstate.h"
#include "state_internals.h"
#include "state_extensionfuncs.h"

EOF

print "void crState".$name."Diff(CR".$name."Bits *b, GLbitvalue bitID,\n";
print "\tCR".$name."State *from, CR".$name."State *to)\n{\n";
gendiffcode ("state_".lc($name).".txt", $name, 1, 0);
print "}\n\n";

print "void crState".$name."Switch(CR".$name."Bits *b, GLbitvalue bitID,\n";
print "\tCR".$name."State *from, CR".$name."State *to)\n{\n";
gendiffcode ("state_".lc($name).".txt", $name, 0, 1);
print "}\n\n";

sub gendiffcode {

$fname = $_[0];
$state_name = $_[1];
$docopy = $_[2];
$doinvalid = $_[3];

$target = "to";
$current = "from";
$bit = "b";
$tab = "\t";
$current_guard = "";
$current_dependancy = "";

%v_types = (
	'l' => 'GLboolean',
	'b' => 'GLbyte',
	'ub' => 'GLubyte',
	's' => 'GLshort',
	'us' => 'GLushort',
	'i' => 'GLint',
	'ui' => 'GLuint',
	'f' => 'GLfloat',
	'd' => 'GLdouble'
);


open(FILE, $fname) || die "Can't open ".$fname."!";

print "\tGLbitvalue nbitID = ~bitID;\n";

mainloop: while ($line = <FILE>) {
	chomp $line;

	if ($line =~ /^#.*/) {
		next mainloop;
	}

## Handle text dump
	if ($line =~ /^\+(.*)/) {
		next mainloop if $doinvalid;
		$line = $1;
	}

	elsif ($line =~ /^-(.*)/) {
		next mainloop if $docopy;
		$line = $1;
	}

	if ($line =~ /^>(.*)/) {
		$text = $1;
		chop $tab if ($line =~ /}/);
		print $tab.$text."\n";
		$tab=$tab."\t" if ($line =~ /{/);
		next mainloop;
	}
  
## Handle commands

	$target = $1 if ($line =~ /%target=(\w*)/);
	$current = $1 if ($line =~ /%current=(\w*)/);
	$bit = $1 if ($line =~ /%bit=(\w*)/);

	if ($line =~ /%flush/) {
		if (($current_guard ne "")) {
			print $tab."$bit->$current_guard &= nbitID;\n";
			chop ($tab);
			print $tab."}\n";
		}
		if ($docopy && ($current_dependancy ne "")) {
			chop ($tab);
			print $tab."}\n";
		}
		$current_guard = "";
		$current_dependancy = "";
	}
	next mainloop if ($line =~ /%.*/);

## Load the line
	($dependancy, $guardbit, $members, $func) = split /:/, $line;
	chomp $func;

## Close the guardbit and dependancy
  if (($current_guard ne "") && ($current_guard ne $guardbit)) {
	print $tab."$bit->$current_guard &= nbitID;\n";
	chop ($tab);
	print $tab."}\n";
  }
  if ($docopy && ($current_dependancy ne "") && ($current_dependancy ne $dependancy)) {
	chop ($tab);
	print $tab."}\n";
  }

## Open the dependancy if
  if ($docopy && $current_dependancy ne $dependancy && $dependancy ne "") {
    print $tab."if ($target->$dependancy)\n".$tab."{\n";
	$tab = $tab."\t";
	$current_dependancy = $dependancy;
  }
  
## Open the guard if
  if ($docopy && $current_dependancy ne $dependancy && $dependancy ne "") {
    print $tab."if ($target->$dependancy)\n".$tab."{\n";
		$tab = $tab."\t";
  }
  
  if ($current_guard ne $guardbit && $guardbit ne "") {
		print $tab."if ($bit->$guardbit & bitID)\n".$tab."{\n";
		$tab = $tab."\t";
		if (substr($members,0,1) ne "\*" && $guardbit eq "enable") {
			print $tab."glAble able[2];\n";
			print $tab."able[0] = diff_api.Disable;\n";
			print $tab."able[1] = diff_api.Enable;\n";
		}
  }

  $current_dependancy = $dependancy;
  $current_guard = $guardbit;

## Handle text dump
  if (substr($members,0,1) eq "\*") {
    print $tab.substr($members, 1)."\n"
  } else {

	## Parse the members variable
	  @mainelem = split /,/, $members;
	  @elems = split /\|/, $members;
	  if ($#elems) {
		@mainelem = ("");
		$mainelem[0] = $elems[0];
		@elems = split /,/, $elems[1];
		foreach $elem (@elems) {
		  $elem = $mainelem[0] . "." . $elem;
		}
	  } else {
		@elems = split /,/, $members;
	  }

	## Check member values
	  if ($guardbit ne "extensions") {
	  print $tab."if (";
	  $first = 1;
	  foreach $elem (@elems) {
		if ($first != 1) {
		  print " ||\n$tab    ";
		}
		$first = 0;
		print "$current->$elem != $target->$elem";
	  }
	  print ")\n".$tab."{\n";
	  $tab = $tab."\t";
	  }

## Handle text function 
	if (substr($func, 0, 1) eq "*") {
		$func = substr($func, 1);
		print $tab.$func."\n";
	} else {
		if ($func ne "") {
## Call the glhw function
			if ($guardbit eq "enable") {
				print $tab."able[$target->$elems[0]]($func);\n";
			} elsif ($guardbit eq "extensions") {
				print $tab."crState$state_name";
				if ($docopy == 1) {
					print "Diff";
				} else {
					print "Switch";
				}
				print "Extensions( from, to );\n";
			} else {
				@funcargs = split /,/, $func;
				$func = shift @funcargs;

				if (substr ($func, -1) eq "v") {
					$v_type = substr ($func, -2, 1);
					$num_elems = $#elems+1;
					print $tab.$v_types{$v_type}." varg[$num_elems];\n";
					$i = 0;
					foreach $elem (@elems) {
						print $tab."varg[$i] = $target->".$elem.";\n";
						$i++;
					}
				}

				print $tab."diff_api.$func (";
				foreach $funcarg (@funcargs) {
					print $funcarg.", ";
				}

## Handle vargs
				if (substr ($func, -1) eq "v") {
					print "varg";
				} else {
					$first = 1;
					foreach $elem (@elems) {
					if ($first != 1) {
						print ",\n$tab    ";
					}
					$first = 0;
					print "$target->$elem";
					}
				}
				print ");\n";
			}
		}
	}

## Do the sync if nessesary
	  if ($docopy and $guardbit ne "extensions") {
		foreach $elem (@mainelem) {
		  print $tab."$current->$elem = $target->$elem;\n";
		}
	  }

	## Do the clear if nessesary
	  if ($doinvalid) {
		if ($guardbit ne "") {
			print $tab."$bit->$guardbit=GLBITS_ONES;\n";
		}
		print $tab."$bit->dirty=GLBITS_ONES;\n";
	  }

	## Close the compare
	  if ($guardbit ne "extensions") {
		  chop ($tab);
		  print $tab."}\n";
	  }
	}
}

## Do final closures
if ($current_guard ne "") {
  print $tab."$bit->$current_guard &= nbitID;\n";
  chop ($tab);
  print $tab."}\n";
}
if ($docopy && $current_dependancy ne "") {
  chop ($tab);
  print $tab."} \/*$current_dependancy*\/\n"
}

print $tab."$bit->dirty &= nbitID;\n";

}

