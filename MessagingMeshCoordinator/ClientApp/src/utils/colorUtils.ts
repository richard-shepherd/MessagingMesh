/**
 * Hash-based color generation utilities
 * Provides deterministic, visually pleasing pastel colors for strings
 */

/**
 * Simple string hash function (djb2 algorithm)
 */
function hashString(str: string): number {
  let hash = 5381;
  for (let i = 0; i < str.length; i++) {
    hash = ((hash << 5) + hash) + str.charCodeAt(i);
  }
  return Math.abs(hash);
}

/**
 * Generate a pastel color from a string using HSL color space
 * @param str - Input string to hash
 * @param saturation - Saturation percentage (default 35 for soft pastels)
 * @param lightness - Lightness percentage (default 80 for light colors)
 * @returns HSL color string
 */
export function generatePastelColor(
  str: string,
  saturation: number = 65,
  lightness: number = 90
): string {
  const hash = hashString(str);
  const hue = hash % 360;
  return `hsl(${hue}, ${saturation}%, ${lightness}%)`;
}

/**
 * Generate a gradient from a string with two related hues
 * @param str - Input string to hash
 * @returns CSS linear gradient string
 */
export function generatePastelGradient(str: string): string {
  const hash = hashString(str);
  const hue = hash % 360;
  const hue2 = (hue + 30) % 360; // Offset by 30 degrees for related color
  
  const saturation = 65;
  const lightness = 90;
  
  const color1 = `hsl(${hue}, ${saturation}%, ${lightness}%)`;
  const color2 = `hsl(${hue2}, ${saturation}%, ${lightness - 5}%)`;
  
  return `linear-gradient(135deg, ${color1} 0%, ${color2} 100%)`;
}
