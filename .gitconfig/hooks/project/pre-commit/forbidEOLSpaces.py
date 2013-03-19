#! /usr/bin/env python
import sys, os, subprocess, re

REPO_DIR = "."

def main(argv):
	git = Git(REPO_DIR)
	stagedFiles = git.staged()
	ignorePatterns = getIgnorePatterns()
	badFiles = []

	for fileName in stagedFiles:
		if os.path.exists(fileName) and git.textFile(fileName) and isBadFile(fileName):
			badFiles.append(fileName)

	if len(badFiles) > 0:
		print >> sys.stderr, "The following files have eol spaces:\n"
		print >> sys.stderr, '\n'.join(badFiles)
		print >> sys.stderr, "\nAborting commit."
		exit(-1)

def ignoreFile(fileName, ignorePatterns):
	for pattern in ignorePatterns:
		if re.match(pattern, fileName):
			return True
	return False

def getIgnorePatterns():
	ignoreFile = os.path.join(REPO_DIR, ".gitconfig", "hooks", "project", ".eolignore")
	content = ""
	if os.path.exists(ignoreFile):
		with open(ignoreFile, "r") as f:
			content = f.read()
	patterns = content.split("\n")
	patterns = [pattern for pattern in patterns if pattern != ""]
	return patterns

def isBadFile(filePath):
	with open(filePath, "r") as f:
		content = f.read()
	match = re.search(r"[\x09\x20]+$", content, re.MULTILINE)
	return match is not None

class Git:
	def __init__(self, repoDir="."):
		self.dir = repoDir
		os.chdir(self.dir)
		self.head = self._head()

	def staged(self):
		files = self._command(["diff", "--name-only", "--staged"])
		return files.split()

	def textFiles(self):
		files = self._command(["grep", "-I", "--name-only", ""])
		return files.split()

	def textFile(self, filePath):
		with open(filePath, "rb") as f:
			buffer = f.read(8000)
		return "\x00" not in buffer

	def _head(self):
		gotHead = self._try(["rev-parse", "--verify", "HEAD"])
		if gotHead:
			return "HEAD"
		else:
			return "4b825dc642cb6eb9a060e54bf8d69288fbee4904"

	def _command(self, cmd):
		cmd.insert(0, "git")
		p = subprocess.Popen(cmd, stdout=subprocess.PIPE, cwd=self.dir)
		stdout, _ = p.communicate()
		return stdout

	def _try(self, cmd):
		cmd.insert(0, "git")
		p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=self.dir)
		p.wait()
		return p.returncode != 0

if __name__ == "__main__":
	main(sys.argv[1:])
