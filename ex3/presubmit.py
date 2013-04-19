#!/usr/bin/python
import sys
import os
import tarfile

EXERCISE_NUMBER = 3

# exit error codes
ERROR_INVALID_USAGE = 1
ERROR_INVALID_FILENAME = 2
ERROR_INVALID_TAR = 3

# set of required files for the exercise
REQUIRED_FILES = set(["winmine_freezed.exe", "winmine_extreme.exe", "winmine_countdown.exe", "trainer.c", "trainer.h", "README"])
OPTIONAL_FILES = set(["wackymine.exe", "winmine.idb", "godlymine.exe"])

def checkFile(filename):
	"""Checks the given tar file against the hard coded file list and verifies README
	format."""
	isWell = True
	tar = tarfile.TarFile(filename)

	# verify file names
	names = set([f.name for f in tar.getmembers()])
	for name in REQUIRED_FILES.difference(names):
		print "[-] File %s should be contained in this .tar file." % name
		isWell = False
	for name in names.difference(REQUIRED_FILES.union(OPTIONAL_FILES)):
		print "[-] File %s should not be contained in this .tar file." % name
		isWell = False

	# verify README
	if "README" in names:
		if not checkReadme(tar.extractfile("README").read()):
			isWell = False

	tar.close()

	return isWell

def checkReadme(readme):
	"""Verifies that the README file contains in its first two rows the usernames,
	ids and emails of the submitters."""

	# take first two lines of readme
	readme = readme.split("\n")[:2]

	for userInfo in readme:
		try:
			username, userid, usermail = [item.strip() for item in userInfo.split(",")]

			if not userid.isdigit():
				print "[-] User ID should contain digits only: %s" % userid
				return False
			elif not "@" in usermail:
				print "[-] User Email should contain the '@' character: %s" % useremail
				return False
			
			print "[+] User: %s, ID: %s, Email: %s" % (username, userid, usermail)
		except ValueError:
			print "[-] Incorrect formatting of header line: %s" % userInfo
			return False
	
	return True

if __name__ == "__main__":
	if len(sys.argv) != 2:
		print "Usage: %s <exercise tar file>" % os.path.basename(sys.argv[0])
		sys.exit(ERROR_INVALID_USAGE)
	if os.path.basename(sys.argv[1]) != ("ex%d.tar" % EXERCISE_NUMBER):
		print "[-] Submitted filename should be ex%d.tar" % EXERCISE_NUMBER
		sys.exit(ERROR_INVALID_FILENAME)
	
	if not checkFile(sys.argv[1]):
		print "[-] You have some errors :("
		sys.exit(ERROR_INVALID_TAR)

	print "[+] All files are present :)"