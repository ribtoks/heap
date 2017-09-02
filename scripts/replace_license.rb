#!/usr/bin/ruby

gpl = <<GPL
/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU Lesser General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
GPL

mpl = <<MPL
/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
MPL

filepath = ARGV[0]

text = File.read(filepath)

replace = text.sub(gpl, mpl)

if replace.length == text.length
  puts "Not found in " + filepath
else
  File.open(filepath, "w") {|file| file.puts replace}
end


## find . ! -path '*build-*' \( -name "*.cpp" -o -name "*.h" -o -name "*.qml" \) -exec dos2unix {} \;
# find . ! -path '*build-*' \( -name "*.cpp" -o -name "*.h" -o -name "*.qml" \) -exec replace_license.rb {} \;
